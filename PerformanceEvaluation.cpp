#include "stdafx.h"
#include "PerformanceEvaluation.h"

bool compareFrames(FrameTransition ft1, FrameTransition ft2)
{
	return (ft1.start < ft2.start);

}
Metrics evaluateResults(vector<Mat> data, vector<FrameTransition> expectedPositives, vector<FrameTransition> actualPositives, TransitionType transitionType) {
	int truePositives = 0;
	int falsePositives = 0;
	int trueNegatives = 0;
	int falseNegatives = 0;
	
	float accuracy = 0;
	float recall = 0;
	float f1Score = 0;
	float precision = 0;

	vector<int> expected = extractFrameIndices(expectedPositives, transitionType);
	vector<int> actual = extractFrameIndices(actualPositives, transitionType);

	// find matches i.e. "True positives"
	
	int ne = expectedPositives.size();
	int na = actualPositives.size();
	int e = 0, a = 0;
	while (e < ne && a < na) {
		if (expected[e] == actual[a]) {
			truePositives++;
			e++;
			a++;
		}
	}
	
	int total = data.size();
	falsePositives = (na - truePositives);
	falseNegatives = (ne - na);
	trueNegatives = total - truePositives - falsePositives - falseNegatives;

	precision = (float)truePositives / (float)(truePositives + falsePositives);
	recall = (float)truePositives / (float)(truePositives + falseNegatives);
	accuracy = (float)(truePositives + trueNegatives) / (float)(truePositives + trueNegatives + falsePositives + falseNegatives);
	f1Score = (float) 2 * precision * recall / (float)(precision + recall);

	return { accuracy, precision, recall, f1Score };
}

vector<int> extractFrameIndices(vector<FrameTransition> results, TransitionType transitionType) {
	vector<int> frameIndices;
	int n = results.size();
	for (int i = 0; i < n; i++) {
		if (results[i].type == transitionType) {
			for (int s = results[i].start; s <= results[i].end; s++) {
				frameIndices.push_back(s);
			}
		}
	}
	// remove duplicates before sorting
	frameIndices.erase(unique(frameIndices.begin(), frameIndices.end()), frameIndices.end());
	sort(frameIndices.begin(), frameIndices.end());

	return frameIndices;
}

void filterResults(vector<FrameTransition>& results, TransitionType type) {
	switch (type) {
		case CUT: {
			remove_if(results.begin(), results.end(), [](FrameTransition ft) {return ft.type == CUT; });
			break; 
		}
		case GRADUAL: {
			remove_if(results.begin(), results.end(), [](FrameTransition ft) {return ft.type == GRADUAL; });
			break;
			}
		case FADE_IN: {
			remove_if(results.begin(), results.end(), [](FrameTransition ft) {return ft.type == FADE_IN; });
			break;
			}
		case FADE_OUT: {
			remove_if(results.begin(), results.end(), [](FrameTransition ft) {return ft.type == FADE_OUT; });
			break;
			}
		case DISSOLVE: {
			remove_if(results.begin(), results.end(), [](FrameTransition ft) {return ft.type == DISSOLVE; });
			break;
		}
	}
}