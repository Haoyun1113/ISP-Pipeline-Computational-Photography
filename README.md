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








