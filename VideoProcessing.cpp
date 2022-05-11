#include "stdafx.h"
#include "PBAalgorithms.h"
#include "HBAalgorithms.h"
#include "EBAalgorithms.h"
#include "MBAalgorithms.h"
#include <fstream>
#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <filesystem>

#define NR_VARIANTS 9

namespace fs = std::filesystem;

const char videoFilePath[] = "Videos/Megamind.avi";
const char videoAllFrames[] = "Videos/keyFrames/all";

char outputDirPaths[][MAX_PATH] = { 
	// PBA test results
	"Videos/keyFrames/PBA_1" , 
	"Videos/keyFrames/PBA_2", 
	"Videos/keyFrames/PBA_3", 
	"Videos/keyFrames/PBA_4",
	// HBA test results
	"Videos/keyFrames/HBA_1" ,
	"Videos/keyFrames/HBA_2",
	"Videos/keyFrames/HBA_3",
	// EBA test results
	"Videos/keyFrames/EBA_1",
	// MBA test results
	"Videos/keyFrames/MBA_1",
};

vector<Shot> selectAlgorithm(int op, ofstream& logFile);
void saveAllFrames();

int main() {
	
	// extract and save all frames from the video sequence
	//saveAllFrames();

	int op;

	do
	{
		system("cls");
		destroyAllWindows();
		cout << "Menu:" << endl;
		cout << " 1 - PBA_v1: single custom threshold" << endl;
		cout << " 2 - PBA_v2: multiple thresholds" << endl;
		cout << " 3 - PBA_v3: multiple thresholds and noise filtering" << endl;
		cout << " 4 - PBA_v4: adaptive thresholds (sliding window)" << endl;
		cout << " 5 - HBA_v1: bin-to-bin difference" << endl;
		cout << " 6 - HBA_v2: histogram intersection" << endl;
		cout << " 7 - HBA_v3: quick shot search" << endl;
		cout << " 8 - EBA_v1: edge change ratio" << endl;
		cout << " 9 - MBA_v1: block matching" << endl;
		cout << " 0 - Exit" << endl;
		cout << "Option: " << endl;
		cin >> op;

		if (op < 1 || op > NR_VARIANTS) {
			continue;
		}

		// clean and remove the output directory
		fs::remove_all(outputDirPaths[op-1]);
		// create a new output directory to store the detected keyframes
		fs::create_directory(outputDirPaths[op-1]);

		char logsFilePath[] = "logs/logs_%d.txt";
		char genericFileName[] = "/%s_shot_%d.jpg";

		// generate a logs file to store the run parameters
		sprintf(logsFilePath, logsFilePath, op);
		ofstream logFile(logsFilePath, ofstream::out | ofstream::app);
		time_t now = time(0);
		logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
		logFile << "<<<< " << ctime(&now);
		logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

		// declare a vector of keyframes which will be populated by the selected shot detection method
		vector<Shot> frames = selectAlgorithm(op, logFile);

		for (auto f : frames) {
			// compute the output path for the current frame
			char outputFile[MAX_PATH], outputFileName[MAX_PATH];
			int index = f.index;
			strcpy(outputFile, outputDirPaths[op - 1]);
			strcat(outputFile, genericFileName);
			sprintf(outputFileName, outputFile, enumToString(f.type), index);

			// save the current frame
			imwrite(outputFileName, f.keyFrame);
		}
		logFile.close();
		waitKey(0);

	} while (op != 0);

	return 0;
}


vector<Shot> selectAlgorithm(int op, ofstream& logFile) {

	switch (op)
	{
		case 1: {
			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			return PBA_v1(videoFilePath, threshold, logFile);
		}
		case 2: {
			float threshold1, threshold2;
			cout << "threshold1 = ";
			cin >> threshold1;

			cout << "threshold2 = ";
			cin >> threshold2;

			return PBA_v2(videoFilePath, threshold1, threshold2, logFile);
		}
		case 3: {
			float threshold1, threshold2;
			cout << "threshold1 = ";
			cin >> threshold1;

			cout << "threshold2 = ";
			cin >> threshold2;

			return PBA_v3(videoFilePath, threshold1, threshold2, logFile);
		}
		case 4: {
			float M, N;

			cout << "Bigger sliding window size = ";
			cin >> M;

			cout << "Smaller sliding window size = ";
			cin >> N;

			return PBA_v4(videoFilePath, M, N, logFile);
		}
		case 5: {
			// todo: change it to use TH and TL thresholds! (HD > TH => cut, TL < HD < TH => ST)

			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			return HBA(videoFilePath, threshold, logFile, BIN_TO_BIN_DIFFERENCE);
		}
		case 6: {
			// todo: change it to use TH and TL thresholds! (HD > TH => cut, TL < HD < TH => ST)

			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			return HBA(videoFilePath, threshold, logFile, HIST_INTERSECTION);
		}
		case 7: {
			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			return HBA_quickShotSearch(videoFilePath, threshold, logFile);
		}
		case 8: {
			float T, M, N;

			cout << "Threshold = ";
			cin >> T;

			cout << "Bigger sliding window size = ";
			cin >> M;

			cout << "Smaller sliding window size = ";
			cin >> N;

			return EBA(videoFilePath, T, N, M, logFile);
		}
		case 9: {
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

void saveAllFrames() {
	fs::remove_all(videoAllFrames);
	fs::create_directory(videoAllFrames);

	// retrieve and store all video frames
	vector<Mat> allFrames = readAllFrames(videoFilePath);

	int n = allFrames.size();
	for (int i = 0; i < n; i++) {
		char outputFile[] = "Videos/keyFrames/all/frame_%ld.jpg";
		sprintf(outputFile, outputFile, i);

		imwrite(outputFile, allFrames[i]);
	}
}
