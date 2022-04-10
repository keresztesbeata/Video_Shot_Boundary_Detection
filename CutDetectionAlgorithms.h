#pragma once
#include "stdafx.h"
#include "RgbConversion.h"

using namespace std;

/**
* PBA = Pixel Based Approach
* used to determine HT (cut);
* Steps:
* 1. Compare 2 corresponding pixels of 2 consecutive video frames;
* 2.The total sum of pixel differences is computed and compared too a threshold
* 3. A transition is declared if the sum exceeds the selected threshold.
* 
* Params: 
*	fileName = the name of the video file to be processed
*	threshold = the threshold value which is used to select the key frames from the frame sequence
* Return value: 
*	key frames representing the cuts
*/
std::vector<cv::Mat> pixelBasedApproach(const char* fileName, float threshold);

/**
* Get the dissimilarity factor between 2 GRAYSCALE images.
* 
* Params: 
*	previousFrame = the previous video frame
*	currentFrame = the current video frame
* Return value:
*	float value representing the dissimilarity factor between the 2 images based on PBA
**/
float getPBADissimilarityForGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame);

/**
* Get the dissimilarity factor between 2 COLOUR images.
*
* Params:
*	previousFrame = the previous video frame
*	currentFrame = the current video frame
* Return value:
*	float value representing the dissimilarity factor between the 2 images based on PBA
**/
float getPBADissimilarityForColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame);
