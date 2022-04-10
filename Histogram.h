#pragma once
#include "RgbConversion.h"
#include "StatisticalProperties.h"

/*--------------------------- compute histogram/ cumulative histogram/ pdf -----------------------*/
int * computeHistogram(Mat_<uchar> img);
int * computeCumulativeHistogram(Mat_<uchar> img);
float * computePDF(Mat_<uchar> img);
float * computeCumulativePDF(Mat_<uchar> img);
int * computeHistogramWithBins(Mat_<uchar> img, int nr_bins);
/*--------------------------- display histogram -----------------------*/
void showHistogram(const char* name, int* hist, const int hist_height, const int hist_cols);
/*--------------------------- get histogram maximas -----------------------*/
std::vector<int> getHistogramMaxima(float* pdf);
int getClosestLocalMaximum(int k, std::vector<int> histMax);

/*---------------------------- image enhancement using histograms -------------------*/
/*
* Determine multiple thresholds for reducing the gray levels:
* Steps:
* 1. determine the local maxima of the histogram
* 2. assign each gray level to the closest max
* 
* Params:
*	img = source image
* Return value:
*	resulting image after applying the algorithm
*/
void multilevelThresholding(Mat_<uchar> img, Mat_<uchar>& result_img);

/*
* Similar to Multilevel thresholding, but it also adds an error offset to the yet unvisited neighburs of the pixel.
* 
* Params:
*	img = source image
* Return value:
*	resulting image after applying the algorithm
*/
void floydSteinbergDithering(Mat_<uchar>& img);

/*
* Using the cumulative PDF, it reassigns new gray levels for the pixels to correct the uneven distribution of the gray levels in the image.
*
* Params:
*	src = source image
* Return value:
*	resulting image after applying the algorithm
*/
Mat_<uchar> histogramEqualization(Mat_<uchar> src);
