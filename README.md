# ISP Pipeline + 3A + Computational Photography

Lets learn theory behind ISP Algorithms, 3A and also Computational Photography Techniques.
---

An Image Signal Processor (ISP) pipeline is a critical component in modern digital cameras and smartphones, responsible for converting raw sensor data into a processed and visually appealing image. Here is an overview of the typical stages in an ISP pipeline:

#### 1. Lens shading Correction(LSC) -
- Corrects vignetting or shading effects caused by the lens.
- Ensures uniform brightness across the image.
- Vignetting is a reduction in brightness at the periphery of an image compared to the center, leading to uneven illumination.

#### 2. Black Level Correction (BLC)

- Removes any bias or offset introduced by the sensor.
- Sets the correct black level for the image.

#### 3. Demosaicing

- Converts the raw Bayer pattern data from the sensor into a full-color image.
- Interpolates the missing color information for each pixel.
-  The Bayer pattern is a color filter array (CFA) used in most digital cameras, consisting of a grid of red, green, and blue filters. Each pixel on the sensor captures only one color component, and demosaicing algorithms are used to interpolate the missing color information for each pixel to produce a complete RGB image.
-  Bilinear Interpolation - 
1. Method: Uses linear interpolation in both horizontal and vertical directions to estimate the missing color values.
2. Process:
For each pixel, average the values of the closest neighboring pixels of the same color.
Interpolate the red, green, and blue channels separately.

![image](https://github.com/user-attachments/assets/4a1031a8-e492-415e-954f-e511fc08c357)


### 4. White Balance (WB)

- Adjusts the colors in the image to make sure white objects appear white under different lighting conditions.
- This correction is necessary because different light sources emit light of different colors (color temperatures), which can cause a color cast in images.

1. Gray World Algorithm
- Method: Assumes that the average color of a scene is gray (i.e., R ≈ G ≈ B).

![image](https://github.com/user-attachments/assets/a8cad723-9d0c-4b39-8e83-d0a3fd7004c3)

2. White patch Algorithm(Max-RGB)
   
![image](https://github.com/user-attachments/assets/6869ea8a-e650-40cd-91d5-e8e45a31edbb)

- Initial Guess:
Use the Gray World Algorithm to get an initial estimate of the white balance correction.
- Refinement:
Apply the White Patch Algorithm if a bright reference white point is detected.


#### 5. Color Correction Matrix (CCM)

 - It typically comes after the demosaicing and white balance adjustment phases in the image processing pipeline.
 - The most commonly used method involves capturing calibration data, computing a transformation matrix, and applying this matrix to the image data to correct color biases.


#### 6. Gamma Correction

- Adjusts the luminance of the image to match the display device's characteristics.
- Helps in achieving a natural look.

#### 7. Noise Reduction (NR)

- Reduces noise introduced by the sensor and other electronic components.
- Common techniques include spatial noise reduction, temporal noise reduction, and chroma noise reduction.
1. Bilateral Filtering
2. Non-Local Means(NLM)
3. Wavelet Based Denosing

---

### Camera Issue -
- SPIKE Investigation: Autotrack System Behavior upon VAL Crash
- investigation involves identifying potential issues, understanding the current system response, and proposing a fix or safety net
- to reproduce the issue, Understand system behavior by forcing a VAL restart during an autotrack session.
- Start an autotrack session.
- Force an exception to cause a VAL restart.
- Observe the system behavior, especially the camera's response and log entries.
- Ensure this fix covers exceptions caused by:
1. Throwing an integer value exception
2. Throwing a VAL-specific exception
3. Throwing a standard exception (std::exception)
- Call GoToHomePosition and stop valTracking in the destructor of the AnalyticsChannel implementation.
- what went wrong ?
- memory leaks -
1. Each time a new tracking session starts, memory is allocated for tracking data and not properly released when the session ends or when an exception occurs.
2. Over time, this can lead to exhaustion of available memory, causing the VAL to crash.
3. Multiple components or threads might attempt to control the PTZ camera simultaneously, leading to conflicts and crashes. hence, we added exceptions in PTZController class and targetTracker.
4. Race conditions might occur when multiple threads access shared resources (e.g., camera position data) without proper synchronization, leading to crashes.
- Solution - Modify the AnalyticsChannel implementation to include a safety net in its destructor.
The destructor should call GoToHomePosition and stop valTracking to ensure the camera returns to the home position and tracking is properly terminated.

#### 8. Camera Imaging Algorithm Development: Applied histogram equalization and CLAHE (Contrast Limited Adaptive
Histogram Equalization) to enhance image contrast.

- Histogram Equalization is a technique used to improve the contrast in an image by effectively spreading out the most frequent intensity values.
- This is done by transforming the intensity values so that the histogram of the output image is approximately uniform.
- Steps:
- Divide the image into small, non-overlapping tiles.
- Apply histogram equalization to each tile.
- Clip the histogram at a predefined threshold to limit contrast enhancement.

- input was ycbcr color space, applies clahe on Y channel,
- The YCrCb color space separates the image into a luminance component (Y) and chrominance components (Cr and Cb).
- By converting the image to the YCrCb color space and applying CLAHE only to the Y (luminance) channel, we enhance the contrast without affecting the color information (Cr and Cb channels).
- The Y channel represents brightness, while Cr and Cb represent color information. This separation allows for contrast enhancement on the brightness component without affecting color information.
what I have done ? with multithreading and concurrancy
1. wrote - CLAHEProcessor Class:
2. Constructor: Initializes the CLAHE processor with a specified clip limit and tile grid size. Creates the CLAHE object using cv::createCLAHE.
3. apply Method: Applies CLAHE to an input image. It checks if the input image is grayscale or color. If grayscale, it applies CLAHE directly. If color, it converts the image to the YCrCb color space, applies CLAHE to the Y channel, By converting the image to the YCrCb color space and applying CLAHE only to the Y (luminance) channel, we enhance the contrast without affecting the color information (Cr and Cb channels)..
4. setClipLimit Method: Allows updating the clip limit of the CLAHE processor.
5. setTileGridSize Method: Allows updating the tile grid size of the CLAHE processor.

![image](https://github.com/user-attachments/assets/6b9cfad8-dea0-4131-8a32-b356e936b99c)


#### 9. Imaging Quality Evaluation: Computed the Modulation Transfer Function (MTF) by implementing an edge detection
algorithm using the Sobel operator and extracted the Edge Spread Function (ESF).

- Edge Detection with Sobel Operator: as sepearble filter. sobel x and sobel y and took the magnitude of this.
- Edge Spread Function (ESF): computeEdgeSpreadFunction computes the ESF by summing the gradient magnitudes along rows (assuming vertical edges for simplicity).
- The ESF is normalized by dividing by its maximum value.
- Line Spread Function (LSF): -calculates the LSF by taking the first derivative of the ESF.
- by subtracting the value from 1d flatten vector of esf.
- computeMTF computes the MTF by taking the Discrete Fourier Transform (DFT) of the LSF.
The MTF values are normalized by dividing by the maximum value.
- MTF50 Calculation:

findMTF50 finds the spatial frequency at which the MTF value drops to 50% of its maximum. This is done using linear interpolation between the points around 0.5.

#### 10. Delta E measures the perceived color difference between two colors. It is commonly used to evaluate color accuracy.
Steps to Implement Delta E Calculation
1. Convert RGB to LAB Color Space:
- Use OpenCV functions to convert images from RGB to LAB color space.
2. Compute Delta E:
- Calculate the Delta E value for each corresponding pixel in the LAB images.
computeDeltaE calculates the Delta E value between corresponding pixels of two LAB images. The Delta E formula used here is the Euclidean distance in the LAB color space:
![image](https://github.com/user-attachments/assets/896788c5-f452-405d-b93b-35149b9d73d4)
- The function calculates the Delta E for each pixel and computes the mean Delta E value for the entire image, providing an overall measure of color fidelity.
- Color fidelity refers to the accuracy with which a camera or display system reproduces the colors of the original scene
- Perceptual Uniformity: LAB is more aligned with human vision. The Euclidean distance between two colors in LAB space (Delta E) is a good approximation of perceived color difference.
-  LAB separates the lightness (L) component from the color information (a and b components),
-  Capture a Reference Image Using a Color Calibration Chart

#### 11. Denosing + Evalution Metrics
- MSE and SSIM(Structural Similarity Index) between the original and denoised frames =[-1,1]
- SSIM assesses the similarity between two images based on three key components: luminance, contrast, and structure.
- mSE considers aboslute errors, SSIM considers changes in structural information, which are more perceptually relevant. 
- Luminance (l): Measures the difference in brightness between the two images.
- Contrast (c): Measures the difference in contrast between the two images.
- Structure (s): Measures the similarity of patterns or structures in the images.

- Steps :
- Calculate Squared Images and Product Image - to compute the local variances and covariance.
- Gaussian blur is applied to smooth the images, effectively calculating local means.
-  Variance and covariance capture the contrast and structural differences between the images.
- Apply Gaussian blur to the squared images and product image, then subtract the squared means to get the variances and covariance.
- Compute the luminance, contrast, and structure components using the calculated means, variances, and covariance.
- The SSIM map provides a pixel-wise similarity measure.

1. Bilateral Filring - Bilateral filtering smooths images while preserving edges by considering both spatial distance and intensity differences.
2. Gaussian Filtering - linear filter - implementes as seperable,
3. Non local means - by averaging pixels with similar neighborhoods. It searches for similar patches within a search window and averages them based on similarity.
- Unlike traditional filters that use only local information, NLM uses a non-local approach by considering the similarity between patches (neighborhoods) of pixels throughout the image.
- Extracting Patches - A patch is a small sub-region of the image centered at a specific pixel.
- Computing the Squared Euclidean Distance: between two patches.
- Computing Weights: The weight w_ij_kl is computed using a Gaussian kernel based on the distance between the patches.
- The denoised pixel value I'(i, j) is obtained by normalizing the weighted sum of pixel values in the search window.


#### 12 High Dynamic Range - 
- This includes aligning images using the Median Threshold Bitmap (MTB) algorithm, calculating the Camera Response Function (CRF),
- merging exposures into an HDR image, and applying tonemapping to create LDR (Low Dynamic Range) images.
- Take several photos of the same scene with different exposure settings (e.g., underexposed - details in the bright areas, correctly exposed, and overexposed-  details in the dark areas).
- Align the images using an algorithm like the Median Threshold Bitmap (MTB) to compensate for any camera movement.
-  The CRF helps in estimating the true radiance values from the pixel values.
-  Select a set of sample pixels from the images. These pixels should cover the full range of intensity values.
-  Optimized with equation that relates the pixel values to the scene radiance using the known exposure times and the CRF.
-  Convert the HDR image to an LDR image using tonemapping algorithms. 
- Common algorithms include:

1. Drago: Suitable for display devices, preserving details and contrast.
2. Reinhard: Mimics the human vision system, providing natural-looking results.
3. Mantiuk: Focuses on contrast enhancement and preserving details.

#### How HDR Works
- HDR works by combining multiple images taken at different exposure levels into a single image that contains a wider range of luminance. This process involves several steps:

1. Capture Multiple Exposures:
- Take several photos of the same scene with different exposure settings (e.g., underexposed - details in the bright areas, correctly exposed, and overexposed-  details in the dark areas).

2. Align the Images:
- Align the images to compensate for any camera movement between shots. This step ensures that corresponding pixels in different exposures refer to the same point in the scene.
Merge Exposures:

3. Combine the aligned images into a single HDR image. This step involves calculating the true radiance values for each pixel by using the information from all the exposures.
4. Tonemapping:
- Convert the HDR image to a Low Dynamic Range (LDR) image that can be displayed on standard monitors or printed. Tonemapping compresses the dynamic range while preserving details and contrast.
