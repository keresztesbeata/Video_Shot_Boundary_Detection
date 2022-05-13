#pragma once
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "Utils.h"

using namespace std;

/**
* MBA = Motion Based Approach
**/

/*------------------------------ MBA with ECR (edge change ratio) ---------------------------------*/
/**
* T = empirical threshold in [0,1]
* N = size of the smaller sliding window
* M = size of the larger sliding window (could be equal to N)
* B = size of the macro block used for comparing consecutive frames
**/
vector<FrameTransition> MBA(const char* fileName, float T, int N, int M, int B, ofstream& logFile);

/**
* BMA = Block Matching Algorithms  
* 
* Compare the block in current video frame to every block in the successive video frames (inside the window w).
* A shot transition is detected if the block difference exceeds the given threshold value T.
* Can detect gardual transitions.
*/


/*---------------------------------------------- TSS = Three Step Search -------------------------------*/
/**
* 
* 
* currentFrame = the current frame to be considered
* prevFrame = the current frame to be considered
* N = size of the block
* T = threshold
* 
* return the min cost, representing the min difference between 2 consecutive matching macro blocks
**/
float TSS(Mat currentFrame, Mat prevFrame, int N);
/**
* Select the index of the frame in a sequence of frames which is a potential key frame due to the high difference compared to neighbour frames (due to motion).
**/
int selectShot_BMA(Mat* frames, int start, int end, int N);

/**
* Cost function: MSE (Mean Squared Error)
* 
* (cx, cy) = center position of the macro block
* N = size of the macro block
**/
float MSE(Mat currentFrame, Mat prevFrame, int cx, int cy, int N);
int getPixelDifference(Vec3b c, Vec3b p);

Mat_<Vec3b> extractBlock(Mat img, int N, int cx, int cy);