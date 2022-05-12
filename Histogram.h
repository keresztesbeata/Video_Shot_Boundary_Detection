#pragma once
#include "RgbConversion.h"
#include "StatisticalProperties.h"

/*--------------------------- compute histogram/ cumulative histogram/ pdf -----------------------*/
int * computeHistogram(Mat_<uchar> img);
int * computeCumulativeHistogram(Mat_<uchar> img);
float * computePDF(Mat_<uchar> img);
float * computeCumulativePDF(Mat_<uchar> img);
int*** computeColorHistogram(Mat_<Vec3b> img);
int * computeHistogramWithBins(Mat_<uchar> img, int nr_bins);
/*--------------------------- display histogram -----------------------*/
void showHistogram(const char* name, int* hist, const int hist_height, const int hist_cols);



