#include "stdafx.h"
#include "PBAalgorithms.h"
#include "HBAalgorithms.h"
#include "EBAalgorithms.h"
#include "MBAalgorithms.h"
#include "GradualTransitionDetector.h"
#include "VideoProcessing.h"

#include <fstream>
#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <filesystem>

#define NR_CUT_DETECTION_ALG 7
#define NR_GRAD_TRANS_DETECTION_ALG 2

namespace fs = std::filesystem;

char parentDir[] = "Videos/gradual_transitions/tests/";
char videoFilePath[] = "Videos/gradual_transitions/fade_transition.mp4";
char videoAllFrames[] = "Videos/gradual_transitions/tests/all";

char outputDirPaths[][MAX_PATH] = {
	// PBA test results
	"/PBA_noise_filt",
	"/PBA_adapt_thresh",
	// HBA test results
	"/HBA_bin3bin",
	"/HBA_hist_int",
	"/HBA_quick_search",
	// EBA test results
	"/EBA_ecr",
	// MBA test results
	"/MBA_bma",
	// FADE in/out
	"/fade"
};

vector<Shot> selectCutDetectionAlgorithm(int op, ofstream& logFile);
vector<GradualTransition> selectGradTransitionAlgorithm(int op, vector<Mat> allFrames, ofstream& logFile);

int main() {

	// retrieve and store all video frames
	vector<Mat> allFrames = readAllFrames(videoFilePath);

	// extract and save all frames from the video sequence
	//saveAllFrames(allFrames, videoFilePath, videoAllFrames);

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
		cout << " 8 - GT: Detect Fade in/out with Std deviation" << endl;
		cout << " 9 - GT: Detect Fade in/out with ECR" << endl;
		cout << " 0 - Exit" << endl;
		cout << "Option: " << endl;
		cin >> op;

		if (op < 1 || op > NR_CUT_DETECTION_ALG + NR_GRAD_TRANS_DETECTION_ALG) {
			continue;
		}

		char outputDir[MAX_PATH];
		strcpy(outputDir, parentDir);
		strcat(outputDir, outputDirPaths[op - 1]);

		// clean and remove the output directory
		fs::remove_all(outputDir);
		// create a new output directory to store the detected keyframes
		fs::create_directory(outputDir);

		char logsFilePath[] = "logs/logs_%d.txt";

		// generate a logs file to store the run parameters
		sprintf(logsFilePath, logsFilePath, op);
		ofstream logFile(logsFilePath, ofstream::out | ofstream::app);
		time_t now = time(0);
		logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		logFile << "<<<< " << ctime(&now);
		logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

		if (op <= NR_CUT_DETECTION_ALG) {
			// declare a vector of keyframes which will be populated by the selected shot detection method
			vector<Shot> keyFrames = selectCutDetectionAlgorithm(op, logFile);
			// save the results
			saveKeyFrames(keyFrames, outputDir);
		}
		else {
			// declare a vector of keyframes which will be populated by the selected gardual transition detection method
			vector<GradualTransition> transitionFrames = selectGradTransitionAlgorithm(op, allFrames, logFile);
			// save the results
			saveTransitionFrames(transitionFrames, allFrames, outputDir);
		}
		
		logFile.close();
		waitKey(0);

	} while (op != 0);

	return 0;
}

vector<Shot> selectCutDetectionAlgorithm(int op, ofstream& logFile) {

	switch (op)
	{
	case 1: {
		float threshold1, threshold2;
		cout << "threshold1 = ";
		cin >> threshold1;

		cout << "threshold2 = ";
		cin >> threshold2;

		return PBA_v3(videoFilePath, threshold1, threshold2, logFile);
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
	default: {
		return vector<Shot>();
	}
	}
}


vector<GradualTransition> selectGradTransitionAlgorithm(int op, vector<Mat> allFrames, ofstream& logFile) {

	switch (op)
	{
		case 8: {
			float maxStdDev, minLength, maxChange;

			cout << "Max standard deviation = ";
			cin >> maxStdDev;

			cout << "Max intensity level change between frames = ";
			cin >> maxChange;

			cout << "Min length of transition = ";
			cin >> minLength;

			return detectFadeTransitions_v1(allFrames, maxStdDev, maxChange, minLength, logFile);
		}
		case 9: {
			float T, minLength;

			cout << "Threshold for ECR = ";
			cin >> T;

			cout << "Min length of transition = ";
			cin >> minLength;

			return detectFadeTransitions_v2(allFrames, T, minLength, logFile);
		}
		default: {
			return vector<GradualTransition>();
		}
	}
}