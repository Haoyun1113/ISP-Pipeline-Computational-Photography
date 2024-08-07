#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>

// Function to compute Sobel edges
void computeSobelEdges(const cv::Mat& src, cv::Mat& grad_x, cv::Mat& grad_y) {
    cv::Sobel(src, grad_x, CV_64F, 1, 0, 3);
    cv::Sobel(src, grad_y, CV_64F, 0, 1, 3);
}

// Function to compute the magnitude of gradients
void computeMagnitude(const cv::Mat& grad_x, const cv::Mat& grad_y, cv::Mat& magnitude) {
    cv::magnitude(grad_x, grad_y, magnitude);
}

// Function to compute the Edge Spread Function (ESF)
void computeEdgeSpreadFunction(const cv::Mat& magnitude, std::vector<double>& esf) {
    esf.resize(magnitude.cols, 0.0);
    for (int i = 0; i < magnitude.rows; ++i) {
        for (int j = 0; j < magnitude.cols; ++j) {
            esf[j] += magnitude.at<double>(i, j);
        }
    }
    // Normalize ESF
    double max_val = *std::max_element(esf.begin(), esf.end());
    for (auto& val : esf) {
        val /= max_val;
    }
}

// Function to compute the Line Spread Function (LSF) by differentiating the ESF
void computeLineSpreadFunction(const std::vector<double>& esf, std::vector<double>& lsf) {
    lsf.resize(esf.size() - 1);
    for (size_t i = 1; i < esf.size(); ++i) {
        lsf[i - 1] = esf[i] - esf[i - 1];
    }
}

// Function to compute the Modulation Transfer Function (MTF) from the LSF
void computeMTF(const std::vector<double>& lsf, std::vector<double>& mtf) {
    cv::Mat lsfMat(lsf);
    cv::Mat dftResult;
    cv::dft(lsfMat, dftResult, cv::DFT_REAL_OUTPUT);

    mtf.resize(dftResult.rows);
    for (int i = 0; i < dftResult.rows; ++i) {
        mtf[i] = std::abs(dftResult.at<double>(i));
    }

    // Normalize MTF
    double max_val = *std::max_element(mtf.begin(), mtf.end());
    for (auto& val : mtf) {
        val /= max_val;
    }
}

int main() {
    cv::Mat src = cv::imread("image.jpg", cv::IMREAD_GRAYSCALE);
    if (src.empty()) {
        std::cerr << "Error: Unable to load image!" << std::endl;
        return -1;
    }

    // Convert to double precision
    src.convertTo(src, CV_64F, 1.0 / 255.0);

    // Step 1: Compute Sobel gradients
    cv::Mat grad_x, grad_y, magnitude;
    computeSobelEdges(src, grad_x, grad_y);
    computeMagnitude(grad_x, grad_y, magnitude);

    // Step 2: Compute Edge Spread Function (ESF)
    std::vector<double> esf;
    computeEdgeSpreadFunction(magnitude, esf);

    // Step 3: Compute Line Spread Function (LSF)
    std::vector<double> lsf;
    computeLineSpreadFunction(esf, lsf);

    // Step 4: Compute Modulation Transfer Function (MTF)
    std::vector<double> mtf;
    computeMTF(lsf, mtf);

    // Display or save the results as needed
    for (size_t i = 0; i < mtf.size(); ++i) {
        std::cout << "Frequency " << i << ": " << mtf[i] << std::endl;
    }

    return 0;
}
