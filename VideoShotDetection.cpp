#include "stdafx.h"
#include "PBAalgorithms.h"
#include "HBAalgorithms.h"
#include "EBAalgorithms.h"
#include "MBAalgorithms.h"
#include "GradualTransitionDetector.h"
#include "VideoProcessing.h"
#include "PerformanceEvaluation.h"

#include <fstream>
#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

#define NR_SHOT_DETECTION_ALG 9

string parentDir = "tests/avatar";
string videoFilePath = "Videos/sample/avatar.mp4";
string expectedResultsFilePath = "tests/avatar/results/expected_results.csv";

string outputDirPaths[] = {
	// PBA test results
	"/PBA_noise_filt",
	"/PBA_adapt_thresh",
	// HBA test results
	"/HBA_bin2bin",
	"/HBA_hist_int",
	"/HBA_quick_search",
	// EBA test results
	"/EBA_ecr",
	// MBA test results
	"/MBA_bma",
	// gradual transition detection
	"/GT_luminance",
	"/GT_ecr",
};

vector<FrameTransition> selectAlgorithm(int op, vector<Mat> allFrames, ofstream& logFile, vector<pair<string, float>>& params);

int main() {

	// retrieve and store all video frames
	vector<Mat> allFrames = readAllFrames(videoFilePath);

	// extract and save all frames from the video sequence
	//saveAllFrames(allFrames, videoFilePath, parentDir);

	int op;

	do
	{
		system("cls");
		destroyAllWindows();
		cout << "Menu:" << endl;
		cout << " --------------------- Cut Detection -------------------- " << endl;
		cout << " 1 - PBA: multiple thresholds and noise filtering" << endl;
		cout << " 2 - PBA: adaptive thresholds (sliding window)" << endl;
		cout << " 3 - HBA: bin-to-bin difference" << endl;
		cout << " 4 - HBA: histogram intersection" << endl;
		cout << " 5 - HBA: quick shot search" << endl;
		cout << " 6 - EBA: edge change ratio" << endl;
		cout << " 7 - MBA: block matching" << endl;
		cout << " --------------- Gradual Transition Detection -------------- " << endl;
		cout << " 8 - GT: Detect Fade in/out based on Histogram difference and varying luminance" << endl;
		cout << " 9 - GT: Detect Gradual Transitions based on edge change ratio" << endl;
		cout << " 0 - Exit" << endl;
		cout << "Option: " << endl;
		cin >> op;

		if (op < 1 || op > NR_SHOT_DETECTION_ALG) {
			continue;
		}

		string outputDir = parentDir + outputDirPaths[op - 1];

		// clean and remove the output directory
		fs::remove_all(outputDir);
		// create a new output directory to store the detected keyframes
		fs::create_directory(outputDir);

		string logsFilePath = parentDir + "/logs/logs_"+ to_string(op) +".txt";
		// generate a logs file to store the run parameters
		ofstream logFile(logsFilePath, ofstream::out | ofstream::app);
		time_t now = time(0);
		logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		logFile << "<<<< " << ctime(&now);
		logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

		vector<pair<string, float>> params;
		// declare a vector of keyframes which will be populated by the selected shot detection method
		vector<FrameTransition> actual = selectAlgorithm(op, allFrames, logFile,params);
		// save the results
		saveKeyFrames(actual, allFrames, outputDir);

		vector<FrameTransition> expected = readExpectedResults(expectedResultsFilePath);
		
		vector<Metrics> results;
		if (op <= 7) {
			results.push_back(evaluateResults(allFrames, expected, actual, CUT));
			results.push_back(evaluateResults(allFrames, expected, actual, GRADUAL));
		}
		else {
			results.push_back(evaluateResults(allFrames, expected, actual, FADE_IN));
			results.push_back(evaluateResults(allFrames, expected, actual, FADE_OUT));
		}
		
		saveResults(results, parentDir, op, params);
		
		logFile.close();
		waitKey(0);

	} while (op != 0);

	return 0;
}

vector<FrameTransition> selectAlgorithm(int op, vector<Mat> allFrames, ofstream& logFile, vector<pair<string, float>>& params) {
	switch (op)
	{
		case 1: {
			float thresholdHigh, thresholdLow;
			cout << "threshold High = ";
			cin >> thresholdHigh;
			params.push_back(make_pair("TH", thresholdHigh));

			cout << "threshold Low = ";
			cin >> thresholdLow;
			params.push_back(make_pair("TL", thresholdLow));

			return PBA_v3(videoFilePath, thresholdHigh, thresholdLow, logFile);
		}
		case 2: {
			float M, N;

			cout << "Bigger sliding window size = ";
			cin >> M;
			params.push_back(make_pair("M", M));

			cout << "Smaller sliding window size = ";
			cin >> N;
			params.push_back(make_pair("N", N));

			return PBA_v4(videoFilePath, M, N, logFile);
		}
		case 3: {
			float threshold;
			cout << "threshold = ";
			cin >> threshold;
			params.push_back(make_pair("T", threshold));

			return HBA(videoFilePath, threshold, logFile, BIN_TO_BIN_DIFFERENCE);
		}
		case 4: {
			float threshold;
			cout << "threshold = ";
			cin >> threshold;
			params.push_back(make_pair("T", threshold));

			return HBA(videoFilePath, threshold, logFile, HIST_INTERSECTION);
		}
		case 5: {
			float threshold;
			cout << "threshold = ";
			cin >> threshold;
			params.push_back(make_pair("T", threshold));

			return HBA_quickShotSearch(videoFilePath, threshold, logFile);
		}
		case 6: {
			float T, M, N;

			cout << "Threshold = ";
			cin >> T;
			params.push_back(make_pair("T", T));

			cout << "Bigger sliding window size = ";
			cin >> M;
			params.push_back(make_pair("M", M));

			cout << "Smaller sliding window size = ";
			cin >> N;
			params.push_back(make_pair("N", N));

			return EBA(videoFilePath, T, N, M, logFile);
		}
		case 7: {
			float T, M, N, B;

			cout << "Threshold = ";
			cin >> T;
			params.push_back(make_pair("T", T));

			cout << "Bigger sliding window size = ";
			cin >> M;
			params.push_back(make_pair("M", M));

			cout << "Smaller sliding window size = ";
			cin >> N;
			params.push_back(make_pair("N", N));

			cout << "Size of macro block = ";
			cin >> B;
			params.push_back(make_pair("B", B));

			return MBA(videoFilePath, T, N, M, B, logFile);
		}
		case 8: {
			float maxStdDev, minLength;

			cout << "Max standard deviation = ";
			cin >> maxStdDev;
			params.push_back(make_pair("maxStdDeviation", maxStdDev));

			cout << "Min length of transition = ";
			cin >> minLength;
			params.push_back(make_pair("minTransitionLength", minLength));

			return detectGradualTransitions_v1(allFrames, maxStdDev, minLength, logFile);
		}
		case 9: {
			float maxStdDev, minLength, maxChange;

			cout << "Threshold for maxStdDev = ";
			cin >> maxStdDev;
			params.push_back(make_pair("maxStdDeviation", maxStdDev));

			cout << "Min length of transition = ";
			cin >> minLength;
			params.push_back(make_pair("minTransitionLength", minLength));

			return detectGradualTransitions_v2(allFrames, maxStdDev, minLength, logFile);
		}
		default: {
			return vector<FrameTransition>();
		}
	}
}