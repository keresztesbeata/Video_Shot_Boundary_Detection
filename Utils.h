#pragma once
#include "common.h"
#include <vector>

using namespace std;
// Check is point is inside the image.
bool isInside(Mat img, int i, int j);

enum ShotType {HARD_CUT, SOFT_CUT};

inline const char* enumToString(ShotType type)
{
    switch (type)
    {
    case HARD_CUT:   return "HC";
    case SOFT_CUT:   return "SC";
    default:      return "";
    }
}

typedef struct _shot {
	int index;
	Mat keyFrame;
	ShotType type;
}Shot;

/*
* Compute different statistical properties of an image in a given frame sequence (region).
*/
float getMeanOfRegion(vector<float> difference, int start, int end);
float getVarianceOfRegion(vector<float> difference, int start, int end, float mean);
float getMaxOfRegion(vector<float> difference, int start, int end);