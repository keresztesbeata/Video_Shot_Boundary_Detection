#pragma once
#include "stdafx.h"
#include "Histogram.h"
#include <fstream>

// histogram difference metrics considered
enum HDmetric {BIN_TO_BIN_DIFFERENCE, HIST_INTERSECTION};

using namespace std;

/*
* HBA = Histogram Based Approach
* Histograms are considered substitutes for PBAs because they do not consider the spatial information of all color spaces.
* Hence, histograms, to some extent, are regarded as invariant to local motion or small global motion compared with PBAs.
* 
* For picture-in-picture transitions, change in small region (CSR), the histograms of two consecutive frames are expected to show similarities because of the minimal change in the frames.
*/
vector<Shot> HBA(const char* fileName, float T, ofstream& logFile, HDmetric metric);

double getHDMetricForColourFrames(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame, int nrBins, HDmetric metric);

/*---------------------------------- version 1: HBA with bin-to-bin difference ---------------------------------------*/
/**
* Steps:
* 1. Sum up the absolute differences between corresponding bins of consecutive frames and compute the average difference value.
* 2. Compare the result against a threshold value, and if it is above the threshold, a shot transition is detected.
**/
double getBinToBinDifference(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins);


/*---------------------------------- version 2: HBA with histogram intersection ---------------------------------------*/
/**
* 1. Compare the freqeuncy value of a gray level in the histograms of 2 consecutive frames and select each time the minimum.
* 2. Calculate the average of these minimum values and subtract it from 1.
**/
double getHistogramIntersection(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins);

/*---------------------------------- version 3: HBA with Quick Search Algorithm -----------------------------------------*/
vector<Shot> HBA_quickShotSearch(const char* fileName, float T, ofstream& logFile);
void quickShotSearch(vector<Mat> frames, Mat leftRep, int leftIdx, Mat rightRep, int rightIdx, int minPartLen, vector<Shot>& shots, double farDissimilarityThreshold, ofstream& logFile);

/**
* GDF = Global Dissimilarity Function
*  => used to compare frames which are far from each other 
*	This shot search algorithm compares the first and the last representative frame of a specified part of the video. If they look the same, the algorithm skips searching inside that part as there is a low chance
	of finding a shot change, this way reducing the computation time. Because the constant change of frame details, in case of non-consecutive frames, the comparison should be based on the public features such as
	background and color histogram.  
*	The GDF returns the distance between two color histograms of representative frames which is computed based on color histogram euclidean distance.
**/
double GDF(Mat previousFrame, Mat currentFrame);

/* Representative frame is used a background extraction filter.*/
Mat computeRepresentativeFrame(Mat * frames, int start, int end);
/**
* LDF looks for a shot change in a local region and it returns frame index if finds it and zero otherwise.
**/
int LDF(Mat* frames, int start, int end);
/**
*	ShotDetector function does a local search in the frame set by calling LDF function. If LDF function finds any shot change it will return its location.
	Afterwards, ShotDetector function will be repeated to compute left representative frame in remained frames in the left part of shot change location and right representative frame in remained
	frames in the right part of shot change location. 
**/
void shotDetector(Mat* frames, int midIdx, int minPartLen, int& shotLoc, Mat& leftMidRep, Mat& rightMidRep);

vector<Mat> readAllFrames(const char* fileName);



