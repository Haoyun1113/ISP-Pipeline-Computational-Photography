#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace cv;
using namespace std;

// Function to compute Mean Squared Error (MSE)
double computeMSE(const Mat& img1, const Mat& img2) {
    Mat s1;
    absdiff(img1, img2, s1);  // |img1 - img2|
    s1.convertTo(s1, CV_32F);  // convert to float
    s1 = s1.mul(s1);  // |img1 - img2|^2
    Scalar s = sum(s1);  // sum elements per channel
    double sse = s.val[0] + s.val[1] + s.val[2];  // sum channels
    double mse = sse / (double)(img1.channels() * img1.total());
    return mse;
}

// Function to compute Structural Similarity Index (SSIM)
Scalar computeSSIM(const Mat& img1, const Mat& img2) {
    const double C1 = 6.5025, C2 = 58.5225;
    Mat img1_32F, img2_32F;
    img1.convertTo(img1_32F, CV_32F);
    img2.convertTo(img2_32F, CV_32F);

    Mat img1_sq = img1_32F.mul(img1_32F);
    Mat img2_sq = img2_32F.mul(img2_32F);
    Mat img1_img2 = img1_32F.mul(img2_32F);

    Mat mu1, mu2;
    GaussianBlur(img1_32F, mu1, Size(11, 11), 1.5);
    GaussianBlur(img2_32F, mu2, Size(11, 11), 1.5);

    Mat mu1_sq = mu1.mul(mu1);
    Mat mu2_sq = mu2.mul(mu2);
    Mat mu1_mu2 = mu1.mul(mu2);

    Mat sigma1_sq, sigma2_sq, sigma12;
    GaussianBlur(img1_sq, sigma1_sq, Size(11, 11), 1.5);
    GaussianBlur(img2_sq, sigma2_sq, Size(11, 11), 1.5);
    GaussianBlur(img1_img2, sigma12, Size(11, 11), 1.5);

    sigma1_sq -= mu1_sq;
    sigma2_sq -= mu2_sq;
    sigma12 -= mu1_mu2;

    Mat t1, t2, t3;
    t1 = 2 * mu1_mu2 + C1;
    t2 = 2 * sigma12 + C2;
    t3 = t1.mul(t2);

    t1 = mu1_sq + mu2_sq + C1;
    t2 = sigma1_sq + sigma2_sq + C2;
    t1 = t1.mul(t2);

    Mat ssim_map;
    divide(t3, t1, ssim_map);
    Scalar mssim = mean(ssim_map);
    return mssim;
}

int main() {
    VideoCapture cap(0);  // Open the default camera
    if (!cap.isOpened()) {
        cerr << "Error: Unable to open the camera!" << endl;
        return -1;
    }

    Mat frame, frameGray, denoisedFrame;
    namedWindow("Original", WINDOW_AUTOSIZE);
    namedWindow("Denoised", WINDOW_AUTOSIZE);

    while (true) {
        cap >> frame;  // Capture a new frame from the camera
        if (frame.empty()) {
            break;  // End of video stream
        }

        cvtColor(frame, frameGray, COLOR_BGR2GRAY);

        // Apply Denoising Filters
        // Bilateral Filter
        bilateralFilter(frameGray, denoisedFrame, 9, 75, 75);

        // Gaussian Filter (uncomment to use)
        // GaussianBlur(frameGray, denoisedFrame, Size(9, 9), 1.5);

        // Non-Local Means Filter (uncomment to use)
        // fastNlMeansDenoising(frameGray, denoisedFrame, 30, 7, 21);

        // Compute Evaluation Metrics
        double mse = computeMSE(frameGray, denoisedFrame);
        Scalar ssim = computeSSIM(frameGray, denoisedFrame);

        // Display the results
        imshow("Original", frameGray);
        imshow("Denoised", denoisedFrame);

        cout << "MSE: " << mse << endl;
        cout << "SSIM: " << ssim[0] << endl;

        if (waitKey(30) >= 0) break;  // Exit on any key press
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
