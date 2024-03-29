#pragma once
#include "stdafx.h"
#include "RgbConversion.h"
#include <fstream>

using namespace std;

/*
* PBA = Pixel Based Approach
* The main drawback of Pixel based approaches (i.e. intensity pixels), whatever the metric used, is that they are unable to differentiate 
* between a large change in a small area and a smaller change in a large area. 
* However, pixel-based techniques are the most sensitive to surrounding disturbances (i.e. noises, illumination changes) that may interfere with a given scene.
*/

/*---------------------------------- version 1: basic PBA based on a dissimilarity factor and custom threshold ---------------------------------------*/

/**
* Steps:
* 1. Compare 2 corresponding pixels of 2 consecutive video frames;
* 2. The average of pixel differences is computed and compared to a threshold
* 3. A transition is declared if the sum exceeds the selected threshold.
* 
* Params: 
*	fileName = the name of the video file to be processed
*	threshold = the threshold value which is used to select the key frames from the frame sequence
* Return value: 
*	key frames representing the cuts and their index in the original frame sequence
**/
vector<FrameTransition> PBA_v1(const char* fileName, float T, ofstream& logFile);

/**
* Get the dissimilarity factor between 2 GRAYSCALE images: DF = Sum(abs(f(n,x,y) - f(n-1,x,y))).
* 
* Params: 
*	previousFrame = the previous video frame
*	currentFrame = the current video frame
* Return value:
*	float value representing the dissimilarity factor between the 2 images based on PBA
**/
float getDFGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame);

/**
* Get the dissimilarity factor between 2 COLOUR images.
**/
float getDFColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame);

/*-------------------------------- version 2: enahnced PBA by selecting a more appropriate threshold/using multiple thresholds --------------------------*/
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
*	T1 = the threshold value which is used for computing the partial differences, T1 should be in [0,255]
* *	T2 = the threshold value which is used to select the key frames from the frame sequence (based on the dissimilarity coefficient), T2 should be in [0,1]
* Return value:
*	key frames representing the cuts
*/
vector<FrameTransition> PBA_v2(string fileName, float T1, float T2, ofstream& logFile);

/**
* Count the pixels which have a dissimilarity factor greater than a given local threshold, applied on GRAYSCALE images.
*
* Params:
*	previousFrame = the previous video frame
*	currentFrame = the current video frame
* Return value:
*	float value representing the dissimilarity factor between the 2 images based on PBA
**/
float getDCGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, float T);
/**
* Count the pixels which have a dissimilarity factor greater than a given local threshold, applied on COLOUR images.
*/
float getDCColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame, float T);

/*------------------------------------------------ version 3: enahnced PBA with noise filtering --------------------------------------------------------*/
/**
* Use an averaging filter (3x3) to replace the pixels in a frame with the average of its neighbours. This is use dto reduce the noise and camera motion effects.
**/
Mat_<uchar> applyAveragingFilterOnGrayScaleImage(Mat_<uchar> src);
Mat_<Vec3b> applyAveragingFilterOnColourImage(Mat_<Vec3b> src);
/**
* PBA = Pixel Based Approach with noise filtering and multiple thresholds
* Pre-processing step:
*	Replace each pixel in the original image with the average of their neighbours in a 3x3 kernel filter.
* Steps: (same as for the multiple thresholding)
* 
* Params:
* *	T1 = the threshold value which is used for computing the partial differences, T1 should be in [0,255]
* *	T2 = the threshold value which is used to select the key frames from the frame sequence (based on the dissimilarity coefficient), T2 should be in [0,1]
*
*/
vector<FrameTransition> PBA_v3(string fileName, float T1, float T2, ofstream& logFile);

/*------------------------------------------------ version 4: adaptive threshold ---------------------------------------------------------*/
/**
* PBA = Pixel Based Approach adaptive thresholds
* Use 2 sliding windows:
*	Bigger sliding window (M frames): determine the average value of the frame difference within this window
*	Smaller sliding window (N frames): detect shot segmentation point
* 
* Remark:
*	- N < M!
*	- the smaller window is inside the bigger one!
*	- typical window sizes would be M = 20, N = 16
*
* Steps: (same as for the multiple thresholding)
*	- T1 = 5 * mean
*	- T2 = 1.5 * mean
*/
vector<FrameTransition> PBA_v4(string fileName, int M, int N, ofstream& logFile);
float getAverageDFFromSlidingWindow(vector<float> difference, int n, int windowSize);
float getMaxDFFromSlidingWindow(vector<float> difference, int n, int windowSize);