#pragma once
#include "common.h"
#include <vector>

using namespace std;

// Check is point is inside the image.
bool isInside(Mat img, int i, int j);

enum TransitionType {CUT, GRADUAL};

inline const char* transitionToString(TransitionType type)
{
    switch (type)
    {
    case CUT:       return "CT";
    case GRADUAL:   return "ST";
    default:        return "";
    }
}

typedef struct _shot {
	int index;
	Mat keyFrame;
	TransitionType type;
}Shot;

/*
* Compute different statistical properties of an image in a given frame sequence (region).
*/
float getMeanOfRegion(vector<float> difference, int start, int end);
float getVarianceOfRegion(vector<float> difference, int start, int end, float mean);
float getMaxOfRegion(vector<float> difference, int start, int end);