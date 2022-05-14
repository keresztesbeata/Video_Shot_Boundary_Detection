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

char parentDir[] = "tests/avatar/";
char videoFilePath[] = "Videos/sample/avatar.mp4";
char genericLogsFilePath[] = "logs/logs_%d.txt";
char genericActualResultsFilePath[] = "tests/avatar/results/actual_%d.txt";
char expectedResultsFilePath[] = "tests/avatar/results/expected.txt";

char outputDirPaths[][MAX_PATH] = {
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

vector<FrameTransition> selectAlgorithm(int op, vector<Mat> allFrames, ofstream& logFile);

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
		cout << " 8 - GT: Detect Fade in/out based on Histogram difference and varying luminence" << endl;
		cout << " 9 - GT: Detect Gradual Transitions based on edge change ratio" << endl;
		cout << " 0 - Exit" << endl;
		cout << "Option: " << endl;
		cin >> op;

		if (op < 1 || op > NR_SHOT_DETECTION_ALG) {
			continue;
		}

		char outputDir[MAX_PATH];
		strcpy(outputDir, parentDir);
		strcat(outputDir, outputDirPaths[op - 1]);

		// clean and remove the output directory
		fs::remove_all(outputDir);
		// create a new output directory to store the detected keyframes
		fs::create_directory(outputDir);

		char logsFilePath[MAX_PATH];
		strcpy(logsFilePath, parentDir);
		strcat(logsFilePath, genericLogsFilePath);

		// generate a logs file to store the run parameters
		sprintf(logsFilePath, logsFilePath, op);
		ofstream logFile(logsFilePath, ofstream::out | ofstream::app);
		time_t now = time(0);
		logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		logFile << "<<<< " << ctime(&now);
		logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

		// declare a vector of keyframes which will be populated by the selected shot detection method
		vector<FrameTransition> actual = selectAlgorithm(op, allFrames, logFile);
		// save the results
		saveKeyFrames(actual, allFrames, outputDir);
		/*
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
		for (auto m : results) {
			saveResults(m, genericActualResultsFilePath, op);
		}
		*/
		logFile.close();
		waitKey(0);

	} while (op != 0);

	return 0;
}

vector<FrameTransition> selectAlgorithm(int op, vector<Mat> allFrames, ofstream& logFile) {
	switch (op)
	{
		case 1: {
			float thresholdHigh, thresholdLow;
			cout << "threshold High = ";
			cin >> thresholdHigh;

			cout << "threshold Low = ";
			cin >> thresholdLow;

			return PBA_v3(videoFilePath, thresholdHigh, thresholdLow, logFile);
		}
		case 2: {
			float M, N;

			cout << "Bigger sliding window size = ";
			cin >> M;

			cout << "Smaller sliding window size = ";
			cin >> N;

			return PBA_v4(videoFilePath, M, N, logFile);
		}
		case 3: {
			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			return HBA(videoFilePath, threshold, logFile, BIN_TO_BIN_DIFFERENCE);
		}
		case 4: {
			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			return HBA(videoFilePath, threshold, logFile, HIST_INTERSECTION);
		}
		case 5: {
			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			return HBA_quickShotSearch(videoFilePath, threshold, logFile);
		}
		case 6: {
			float T, M, N;

			cout << "Threshold = ";
			cin >> T;

			cout << "Bigger sliding window size = ";
			cin >> M;

			cout << "Smaller sliding window size = ";
			cin >> N;

			return EBA(videoFilePath, T, N, M, logFile);
		}
		case 7: {
			float T, M, N, B;

			cout << "Threshold = ";
			cin >> T;

			cout << "Bigger sliding window size = ";
			cin >> M;

			cout << "Smaller sliding window size = ";
			cin >> N;

			cout << "Size of macro block = ";
			cin >> B;

			return MBA(videoFilePath, T, N, M, B, logFile);
		}
		case 8: {
			float maxStdDev, minLength;

			cout << "Max standard deviation = ";
			cin >> maxStdDev;

			cout << "Min length of transition = ";
			cin >> minLength;

			return detectGradualTransitions_v1(allFrames, maxStdDev, minLength, logFile);
		}
		case 9: {
			float maxStdDev, minLength, maxChange;

			cout << "Threshold for maxStdDev = ";
			cin >> maxStdDev;

			cout << "Min length of transition = ";
			cin >> minLength;

			return detectGradualTransitions_v2(allFrames, maxStdDev, minLength, logFile);
		}
		default: {
			return vector<FrameTransition>();
		}
	}
}