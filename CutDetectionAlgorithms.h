#pragma once
#include "stdafx.h"
#include "RgbConversion.h"
#include <fstream>

using namespace std;

/**
* PBA = Pixel Based Approach
* used to determine HT (cut);
* Steps:
* 1. Compare 2 corresponding pixels of 2 consecutive video frames;
* 2. The average of pixel differences is computed and compared too a threshold
* 3. A transition is declared if the sum exceeds the selected threshold.
* 
* Params: 
*	fileName = the name of the video file to be processed
*	threshold = the threshold value which is used to select the key frames from the frame sequence
* Return value: 
*	key frames representing the cuts and their index in the original frame sequence
**/
vector<pair<int,Mat>> pixelBasedApproach(const char* fileName, float threshold, ofstream& logFile);

/*--------------------- basic PBA based on a dissimilarity factor and custom threshold --------------------------*/
/**
* Get the dissimilarity factor between 2 GRAYSCALE images.
* 
* Params: 
*	previousFrame = the previous video frame
*	currentFrame = the current video frame
* Return value:
*	float value representing the dissimilarity factor between the 2 images based on PBA
**/
float getDissimilarityFactorForGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame);

/**
* Get the dissimilarity factor between 2 COLOUR images.
**/
float getDissimilarityFactorForColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame);

/*--------------------- enahnced PBA by selecting a more appropriate threshold/using multiple thresholds --------------------------*/
/**
* Enhanced PBA to reduce the disturbance in the dissimilarity signal using partial differences and multiple thresholds.
* Steps:
* 1. Compare 2 corresponding pixels of 2 consecutive video frames and get the absolute difference between their intensity levels.
*	 If the partial difference exceeded threshold1, then that pixel is considered a change.
* 2. Sum up all the partial differences of the pixels and divide by the number of pixels to obtain the average.
*	 If the result exceeded threshold2, then a cut is detected.
*
* Params:
*	fileName = the name of the video file to be processed
*	threshold1 = the threshold value which is used for computing the partial differences
* *	threshold2 = the threshold value which is used to select the key frames from the frame sequence
* Return value:
*	key frames representing the cuts
*/
vector<pair<int, Mat>> pixelBasedApproachWithMultipleThresholds(const char* fileName, float threshold1, float threshold, ofstream& logFile);

/**
* Count the pixels which have a dissimilarity factor greater than a given local threshold, applied on GRAYSCALE images.
*
* Params:
*	previousFrame = the previous video frame
*	currentFrame = the current video frame
* Return value:
*	float value representing the dissimilarity factor between the 2 images based on PBA
**/
float getDissimilarityCountForGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, float threshold);
/**
* Count the pixels which have a dissimilarity factor greater than a given local threshold, applied on COLOUR images.
*/
float getDissimilarityCountForColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame, float threshold);

/*--------------------- enahnced PBA with noise filtering --------------------------*/
/**
* Use an averaging filter (3x3) to replace the pixels in a frame with the average of its neighbours. This is use dto reduce the noise and camera motion effects.
**/
Mat_<uchar> applyAveragingFilterOnGrayScaleImage(Mat_<uchar> src);
Mat_<Vec3b> applyAveragingFilterOnColourImage(Mat_<Vec3b> src);
/**
* PBA = Pixel Based Approach with noise filtering and multiple thresholds
* Pre-processing step:
*	Replace each pixel in the original image with the average of their neighbours in a 3x3 kernel filter.
* 
* Steps: (same as for the multiple thresholding)
*/
vector<pair<int, Mat>> pixelBasedApproachWithMultipleThresholdsAndNoiseFiltering(const char* fileName, float threshold1, float threshold, ofstream& logFile);