#include "stdafx.h"
#include "PerformanceEvaluation.h"
#include <fstream>

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

	vector<int> actual = extractFrameIndices(actualPositives, transitionType);
	vector<int> expected = extractFrameIndices(expectedPositives, transitionType);

	// find matches i.e. "True positives"
	int ne = expected.size();
	int na = actual.size();

	int e = 0, a = 0;
	while (e < ne && a < na) {
		if (expected[e] == actual[a]) {
			truePositives++;
			e++;
			a++;
		}
		else if (expected[e] < actual[a]) {
			e++;
			falsePositives++;
		}
		else {
			a++;
			falseNegatives++;
		}
	}

	while (e < ne) {
		falsePositives++;
		e++;
	}
	while (a < na) {
		falseNegatives++;
		a++;
	}

	if (a == 0 && e == 0) {
		goto FINISH;
	}

	int total = data.size();
	trueNegatives = total - truePositives - falsePositives - falseNegatives;

	if (truePositives > 0 || falsePositives > 0) {
		precision = (float)truePositives / (float)(truePositives + falsePositives);
	}
	if (truePositives > 0 || falseNegatives > 0) {
		recall = (float)truePositives / (float)(truePositives + falseNegatives);
	}
	if (total > 0) {
		accuracy = (float)(truePositives + trueNegatives) / (float)total;
	}
	if (precision > 0 || recall > 0) {
		f1Score = (float)2 * precision * recall / (float)(precision + recall);
	}

FINISH:
	return { transitionType, accuracy, precision, recall, f1Score };
}

vector<int> extractFrameIndices(vector<FrameTransition> results, TransitionType transitionType) {
	vector<int> frameIndices;
	for (auto r:results) {
		if (r.type == transitionType) {
			for (int s = r.start; s <= r.end; s++) {
				frameIndices.push_back(s);
			}
		}
	}
	if (frameIndices.size() > 0) {
		// remove duplicates before sorting
		frameIndices.erase(unique(frameIndices.begin(), frameIndices.end()), frameIndices.end());
		sort(frameIndices.begin(), frameIndices.end());
	}

	return frameIndices;
}

void saveResults(vector<Metrics> metrics, string outputDirPath, int op, vector<pair<string, float>> params) {
	string outputFilePath = outputDirPath + "/results/results_" + to_string(op) +".csv";

	fstream f(outputFilePath, fstream::out | fstream::app);

	for (auto p : params) {
		f << p.first << ",";
	}
	f << "type,accuracy,precision,recall,f1Score" << endl;

	for (auto m : metrics) {

		for (auto p : params) {
			f << p.second << ",";
		}
		f << transitionToString((TransitionType)m.type) << ","<< m.accuracy << "," << m.precision << "," << m.recall << "," << m.f1Score << endl;
	}
	f.close();
}

string readFileIntoString(const string& path) {
	auto ss = ostringstream{};
	ifstream input_file(path);
	if (!input_file.is_open()) {
		cerr << "Could not open the file - '"
			<< path << "'" << endl;
		exit(EXIT_FAILURE);
	}
	ss << input_file.rdbuf();
	return ss.str();
}

vector<FrameTransition> readExpectedResults(string expectedResultsFilePath) {
	vector<FrameTransition> frames;
	
	char delimiter = ',';
	string file_contents = readFileIntoString(expectedResultsFilePath);
	istringstream sstream(file_contents);
	std::vector<string> items;
	string record;

	// read the header first
	getline(sstream, record);

	while (getline(sstream, record)) {
		istringstream line(record);
		while (std::getline(line, record, delimiter)) {
			items.push_back(record);
		}
		int start = stoi(items[0]);
		int end = stoi(items[1]);
		int type = stoi(items[2]);
		frames.push_back({ start, end, (TransitionType)type });
		items.clear();
	}

	return frames;
}