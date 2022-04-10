#pragma once
#include "Utils.h"

/* --------------------- separate RGB channels ----------------------------- */
// Extract the rgb values into separate matrices
void extractRGBChannelsFromColourImage(Mat_<Vec3b> img, Mat_<uchar>& r_img, Mat_<uchar>& g_img, Mat_<uchar>& b_img);

/* --------------------- compute value of pixels corresponding to another type (RGB/GrayScale/Binary) ----------------------------- */
// Compute the grayscale value from an RGB value, by taking the average of the 3 colour channels as the new gray level: value = (r + g + b)/3
unsigned char RGBtoGrayScale(Vec3b pixel);
// Compute the binary value from a grayscale level based on a threshold
unsigned char grayScaleToBlackAndWHite(unsigned char grayScaleValue, int threshold);
// Compute the HSV values from the RGB values
Vec3f RGBtoHSV(Vec3b rgb);

/* --------------------- convert images to another type of image (RGB/GrayScale/Binary) ----------------------------- */
// Convert a grayscale image to binary (black and white) based on a threshold
Mat_<uchar> convertGrayScaleToBlackAndWhite(Mat_<uchar> src, int threshold);
// Convert a colour image to grayscale
Mat_<uchar> convertRgbToGrayscale(Mat_<Vec3b> src);
// Convert an RGB image to HSV
void convertRgbToHsv(Mat_<Vec3b> img, Mat_<uchar>& h_img, Mat_<uchar>& s_img, Mat_<uchar>& v_img);

