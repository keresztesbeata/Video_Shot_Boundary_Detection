#pragma once
#include "stdafx.h"
#include "Histogram.h"
#include "RgbConversion.h"

float computeMeanIntensitiyValue(Mat_<uchar> img);
float computeStandardDeviation(Mat_<uchar> img);
Mat_<uchar> basicTresholding(Mat_<uchar> src);

Vec3f computeMeanIntensitiyValueColor(Mat_<Vec3b> img);
Vec3f computeStandardDeviationColor(Mat_<Vec3b> img);