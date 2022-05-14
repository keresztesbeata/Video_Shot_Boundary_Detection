#pragma once
#include "stdafx.h"
#include "Utils.h"
#include <vector>

using namespace std;

typedef struct _metrics {
	float accuracy;
	float precision;
	float recall;
	float f1Score;
}Metrics;

void filterResults(vector<FrameTransition>& results, TransitionType type);
vector<int> extractFrameIndices(vector<FrameTransition> results, TransitionType transitionType);
Metrics evaluateResults(vector<Mat> data, vector<FrameTransition> expectedPositives, vector<FrameTransition> actualPositives, TransitionType transitionType);