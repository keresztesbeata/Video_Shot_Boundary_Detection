#pragma once
#include "Utils.h"
#include <iostream>

Mat cannyEdgeDetector(Mat src);
bool isEdgePixel(Mat img, int i, int j);
