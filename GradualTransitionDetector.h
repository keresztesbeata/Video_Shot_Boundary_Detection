#pragma once
#include "stdafx.h"
#include "Utils.h"
#include <fstream>

/**
* Detect fade transitions using the accumulative histogram difference and the luminance values of the consecutive frames.
**/
vector<FrameTransition> detectGradualTransitions_v1(vector<Mat> frames, float maxStdDev, int minLength, ofstream& logFile);
/**
* Throughout the dissolve transition, the edges of objects gradually vanish, whereas new objectedges become gradually apparent.
Moreover, the edges appear gradually in fade-in transition anddisappear gradually in fade-out transition.
**/
vector<FrameTransition> detectGradualTransitions_v2(vector<Mat> frames, float maxStdDev, int minLength, ofstream& logFile);

vector<FrameTransition> detectGradualTransitions_v3(vector<Mat> frames, float maxStdDev, int minLength, ofstream& logFile);

int getHistDifference(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins);
vector<int> findMonochromeFrames(vector<Mat> frames, float T);
bool isFading(int minPrev, int maxPrev, int minCurr, int maxCurr);