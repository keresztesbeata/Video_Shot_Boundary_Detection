#pragma once
#include "common.h"
#include <vector>

using namespace std;

// Check is point is inside the image.
bool isInside(Mat img, int i, int j);

enum TransitionType {CUT, GRADUAL, FADE_IN, FADE_OUT};

inline const char* transitionToString(TransitionType type)
{
    switch (type)
    {
        case CUT:       return "cut";
        case GRADUAL:   return "gradual";
        case FADE_IN:   return "fade_in";
        case FADE_OUT:   return "fade_out";
        default: return "";
    }
}

typedef struct _shot {
    int start; // start frame index
    int end;  // end frame index
	TransitionType type;
}FrameTransition;

/*
* Compute different statistical properties of an image in a given frame sequence (region).
*/
float getMeanOfRegion(vector<float> difference, int start, int end);
float getVarianceOfRegion(vector<float> difference, int start, int end, float mean);
float getMaxOfRegion(vector<float> difference, int start, int end);