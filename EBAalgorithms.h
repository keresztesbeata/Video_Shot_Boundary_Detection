#pragma once
#include <iostream>
#include <fstream>
#include "EdgeDetection.h"

#define MAX_DIST 50

using namespace std;

/**
* EBA = Edge Based Approach
**/

/*------------------------------ v1: EBA with ECR (edge change ratio) ---------------------------------*/
/**
* T = empirical threshold in [0,1]
* N = size of the smaller sliding window
* M = size of the larger sliding window (could be equal to N)
**/
vector<pair<int, Mat>> EBA_v1(const char* fileName, float T, int N, int M, ofstream& logFile);
float ECR(Mat previousFrame, Mat currentFrame);
float getDistanceToClosestEdgePixel(Mat img, int x, int y, int w);
float getMeanOfRegion(vector<float> difference, int start, int end);
float getVarianceOfRegion(vector<float> difference, int start, int end, float mean);
float getMaxOfRegion(vector<float> difference, int start, int end);