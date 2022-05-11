#pragma once
#include "Utils.h"
#include <iostream>

Mat cannyEdgeDetector(Mat src);
bool isEdgePixel(Mat img, int i, int j);


enum KernelType { PREWITT, SOBEL, ROBERT };
enum EdgeType { STRONG_EDGE, WEAK_EDGE, NON_EDGE };

Mat_<uchar> cannyEdgeDetection(Mat_<uchar> src, float p, float k);

Mat_<uchar> gaussianNoiseFilter(Mat_<uchar> src, int w);
Mat_<float> convolution(Mat_<uchar> src, int w, Mat_<float> H);
void computeGradient(Mat_<uchar> src, KernelType kernel, Mat_<float>& Fx, Mat_<float>& Fy);
void computeGradientMagnitudeAndDirection(Mat_<uchar> src, KernelType kernel, Mat_<float>& mag, Mat_<float>& phi);
// thin the degs by retaining only the edge points with the highest gradient module along the direction of the gradient vector
Mat_<uchar> nonMaximaSuppression(Mat_<uchar> mag, Mat_<float> phi);
int getOrientation(float radian);
Mat_<uchar> normalize(Mat_<float> img, KernelType kernelType);
/*
* p has values between 0.01 and 0.1
*/
float computeAdaptiveThreshold(Mat_<uchar> mag, float p);
/*
* k < 1, for ex. k = 0.4
*/
Mat_<uchar> thresholding(Mat_<uchar> mag, float p, float k);
Mat_<uchar> convert(Mat_<uchar> img);
void edgeExtension(Mat_<uchar>& img);