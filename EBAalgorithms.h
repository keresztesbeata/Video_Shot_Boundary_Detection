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
vector<FrameTransition> EBA(string fileName, float T, int N, int M, ofstream& logFile);
/**
* Compute the edge change ratio between 2 consecutive frames.
**/
float ECR(Mat previousFrame, Mat currentFrame);
void computeECR(Mat previousFrame, Mat currentFrame, float& ecr_in, float& ecr_out);
/**
* Check if there is an edge pixel inside a w x w search boundary.
**/
bool isPointInNeighbourhood(Mat img, int x, int y, int r);

