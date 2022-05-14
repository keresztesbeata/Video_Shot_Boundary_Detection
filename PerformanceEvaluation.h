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

vector<int> extractFrameIndices(vector<FrameTransition> results, TransitionType transitionType);
Metrics evaluateResults(vector<Mat> data, vector<FrameTransition> expectedPositives, vector<FrameTransition> actualPositives, TransitionType transitionType);

void saveResults(Metrics metrics, char* outputDirPath, int op);
vector<FrameTransition> readExpectedResults(char* expectedResultsFilePath);