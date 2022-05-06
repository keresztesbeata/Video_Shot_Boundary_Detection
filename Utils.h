#pragma once
#include "common.h"
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