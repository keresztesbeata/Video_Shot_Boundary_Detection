#pragma once
#include "stdafx.h"
#include "Utils.h"
#include <vector>

using namespace std;

typedef struct _metrics {
	TransitionType type;
	float accuracy;
	float precision;
	float recall;
	float f1Score;
}Metrics;

vector<int> extractFrameIndices(vector<FrameTransition> results, TransitionType transitionType);
Metrics evaluateResults(vector<Mat> data, vector<FrameTransition> expectedPositives, vector<FrameTransition> actualPositives, TransitionType transitionType);

vector<FrameTransition> readExpectedResults(string expectedResultsFilePath);
void saveResults(vector<Metrics> metrics, string outputDirPath, int op, vector<pair<string, float>> params);