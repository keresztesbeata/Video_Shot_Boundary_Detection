#pragma once
#include "stdafx.h"
#include "Histogram.h"
#include <fstream>

// histogram difference metrics considered
enum HDmetric {BIN_TO_BIN_DIFFERENCE, CHI_SQUARE_TEST, HIST_INTERSECTION};

using namespace std;

/*
* HBA = Histogram Based Approach
* Histograms are considered substitutes for PBAs because they do not consider the spatial information of all color spaces.
* Hence, histograms, to some extent, are regarded as invariant to local motion or small global motion compared with PBAs.
* 
* For picture-in-picture transitions, change in small region (CSR), the histograms of two consecutive frames are expected to show similarities because of the minimal change in the frames.
*/
vector<pair<int, Mat>> HBA(const char* fileName, float T, ofstream& logFile, HDmetric metric);
float getHDMetricForColourFrames(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame, int nrBins, HDmetric metric);

/*---------------------------------- version 1: HBA with bin-to-bin difference ---------------------------------------*/
/**
* Steps:
* 1. Sum up the absolute differences between corresponding bins of consecutive frames and compute the average difference value.
* 2. Compare the result against a threshold value, and if it is above the threshold, a shot transition is detected.
**/
float getBinToBinDifference(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins);

/*---------------------------------- version 2: HBA with Chi-square test --------------------------------------------*/
float getChiSquareTest(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins);

/*---------------------------------- version 3: HBA with histogram intersection ---------------------------------------*/
float getHistogramIntersection(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins);


