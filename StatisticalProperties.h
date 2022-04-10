#pragma once
#include "stdafx.h"
#include "Histogram.h"
#include "RgbConversion.h"

float computeMeanIntensitiyValue(Mat_<uchar> img);
float computeStandardDeviation(Mat_<uchar> img);
Mat_<uchar> basicTresholding(Mat_<uchar> src);