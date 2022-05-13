#pragma once
#include "stdafx.h"
#include "Utils.h"
#include <fstream>

vector<FrameTransition> detectFadeTransitions_v1(vector<Mat> frames, float maxStdDev, float maxChange, int minLength, ofstream& logFile);
vector<FrameTransition> detectFadeTransitions_v2(vector<Mat> frames, float maxStdDev, float maxChange, int minLength, ofstream& logFile);
vector<FrameTransition> detectFadeTransitions_v3(vector<Mat> frames, float maxStdDev, int minLength, ofstream& logFile);
vector<int> findMonochromeFrames(vector<Mat> frames, float T);