#pragma once
#include <iostream>
#include <fstream>
#include "EdgeDetection.h"

#define MAX_DIST 50

using namespace std;

/**
* EBA = Edge Based Approach
**/

/*------------------------------  EBA with ECR (edge change ratio) ---------------------------------*/
/**
* T = empirical threshold in [0,1]
* N = size of the smaller sliding window
* M = size of the larger sliding window (could be equal to N)
**/
vector<Shot> EBA(const char* fileName, float T, int N, int M, ofstream& logFile);
/**
* Compute the edge change ratio between 2 consecutive frames.
**/
float ECR(Mat previousFrame, Mat currentFrame);
/**
* Get the distance to the closest edge pixel relative to the current pixel inside a w x w search boundary.
**/
float getDistanceToClosestEdgePixel(Mat img, int x, int y, int w);

