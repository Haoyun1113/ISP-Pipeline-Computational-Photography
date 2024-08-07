#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

class CLAHEProcessor {
public:
    CLAHEProcessor(double clipLimit = 2.0, cv::Size tileGridSize = cv::Size(8, 8))
        : clipLimit(clipLimit), tileGridSize(tileGridSize) {
        clahe = cv::createCLAHE(clipLimit, tileGridSize);
    }

    void apply(cv::Mat& src) const {
        if (src.channels() == 1) {
            // Apply CLAHE to a grayscale image directly
            clahe->apply(src, src);
        } else {
            // Split the YCbCr image into its channels
            std::vector<cv::Mat> channels;
            cv::split(src, channels);

            // Apply CLAHE to the Y (luminance) channel using multithreading
            processChannelInParallel(channels[0]);

            // Merge the channels back
            cv::merge(channels, src);
        }
    }

    void setClipLimit(double newClipLimit) {
        clipLimit = newClipLimit;
        clahe->setClipLimit(clipLimit);
    }

    void setTileGridSize(cv::Size newTileGridSize) {
        tileGridSize = newTileGridSize;
        clahe->setTilesGridSize(tileGridSize);
    }

private:
    double clipLimit;
    cv::Size tileGridSize;
    cv::Ptr<cv::CLAHE> clahe;

    void processTile(cv::Mat& tile) const {
        clahe->apply(tile, tile);
    }

    void processChannelInParallel(cv::Mat& channel) const {
        int tileWidth = channel.cols / tileGridSize.width;
        int tileHeight = channel.rows / tileGridSize.height;

        std::vector<std::thread> threads;
        std::mutex mutex;

        for (int y = 0; y < channel.rows; y += tileHeight) {
            for (int x = 0; x < channel.cols; x += tileWidth) {
                cv::Rect tileRect(x, y, tileWidth, tileHeight);
                cv::Mat tile = channel(tileRect);

                // Launch a thread to process each tile
                threads.emplace_back([this, &tile, &mutex]() {
                    // Ensure thread-safe access to the tile
                    std::lock_guard<std::mutex> lock(mutex);
                    processTile(tile);
                });
            }
        }

        // Wait for all threads to finish
        for (auto& thread : threads) {
            thread.join();
        }
    }
};

int main() {
    cv::Mat src = cv::imread("image.jpg", cv::IMREAD_COLOR);
    if (src.empty()) {
        std::cerr << "Error: Unable to load image!" << std::endl;
        return -1;
    }

    // Convert to YCrCb color space since we assume input is BGR
    cv::Mat ycrcb;
    cv::cvtColor(src, ycrcb, cv::COLOR_BGR2YCrCb);

    CLAHEProcessor claheProcessor(2.0, cv::Size(8, 8));
    claheProcessor.apply(ycrcb);

    // Convert back to BGR color space
    cv::Mat dst;
    cv::cvtColor(ycrcb, dst, cv::COLOR_YCrCb2BGR);

    cv::imshow("Original Image", src);
    cv::imshow("CLAHE Image", dst);
    cv::waitKey(0);

    return 0;
}
