#pragma once
#include "stdafx.h"
#include "Utils.h"
#include <fstream>

enum EffectType {FADE_IN, FADE_OUT, DISSOLVE, WIPE};

inline const char* effectToString(EffectType effect)
{
    switch (effect)
    {
    case FADE_IN:    return "fade_in";
    case FADE_OUT:   return "fade_out";
    case DISSOLVE:   return "dissolve";
    case WIPE:       return "wipe";
    default:         return "";
    }
}

typedef struct _gradualTransition {
	int start; // start frame index
	int end;  // end frame index
	EffectType effect;
}GradualTransition;


vector<GradualTransition> detectFadeTransitions_v1(vector<Mat> frames, float maxStdDev, float maxChange, int minLength, ofstream& logFile);
vector<GradualTransition> detectFadeTransitions_v2(vector<Mat> frames, float T, int minLength, ofstream& logFile);
vector<int> findMonochromeFrames(vector<Mat> frames, float T, int M);