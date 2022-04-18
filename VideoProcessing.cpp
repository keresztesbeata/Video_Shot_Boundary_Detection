#include "stdafx.h"
#include "PBAalgorithms.h"
#include "HBAalgorithms.h"
#include <fstream>
#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

const char videoFilePath[] = "Videos/Megamind.avi";
char outputDirPaths[][MAX_PATH] = { 
	// PBA test results
	"Videos/keyFrames/PBA_1" , 
	"Videos/keyFrames/PBA_2", 
	"Videos/keyFrames/PBA_3", 
	"Videos/keyFrames/PBA_4",
	// HBA test results
	"Videos/keyFrames/HBA_1" ,
	"Videos/keyFrames/HBA_2",
	"Videos/keyFrames/HBA_3", };
const char logsFilePath[] = "logs/hba_logs_1.txt";
char genericFileName[] = "/key_frame_%d.jpg";

int main() {
	int op;
	ofstream logFile(logsFilePath, ofstream::out | ofstream::app);
	time_t now = time(0);
	logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	logFile << "<<<< " << ctime(&now);
	logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

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
		cout << " 6 - HBA_v2: chi-square test" << endl;
		cout << " 7 - HBA_v3: histogram intersection" << endl;
		cout << " 0 - Exit" << endl;
		cout << "Option: " << endl;
		cin >> op;
		switch (op)
		{
		case 1: {
			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			fs::remove_all(outputDirPaths[0]);
			fs::create_directory(outputDirPaths[0]);

			vector<pair<int, Mat>> frames = PBA_v1(videoFilePath, threshold, logFile);
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFile[MAX_PATH], outputFileName[MAX_PATH];
				int index = f.first;
				strcpy(outputFile, outputDirPaths[0]);
				strcat(outputFile, genericFileName);
				sprintf(outputFileName, outputFile, index);

				// save the current frame
				imwrite(outputFileName, f.second);
			}
			waitKey(0);
			break;
		}
		case 2: {
			float threshold1, threshold2;
			cout << "threshold1 = ";
			cin >> threshold1;

			cout << "threshold2 = ";
			cin >> threshold2;

			fs::remove_all(outputDirPaths[1]);
			fs::create_directory(outputDirPaths[1]);

			vector<pair<int, Mat>> frames = PBA_v2(videoFilePath, threshold1, threshold2, logFile);
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFile[MAX_PATH], outputFileName[MAX_PATH];
				int index = f.first;
				strcpy(outputFile, outputDirPaths[1]);
				strcat(outputFile, genericFileName);
				sprintf(outputFileName, outputFile, index);

				// save the current frame
				imwrite(outputFileName, f.second);
			}
			waitKey(0);
			break;
		}
		case 3: {
			float threshold1, threshold2;
			cout << "threshold1 = ";
			cin >> threshold1;

			cout << "threshold2 = ";
			cin >> threshold2;

			fs::remove_all(outputDirPaths[2]);
			fs::create_directory(outputDirPaths[2]);

			vector<pair<int, Mat>> frames = PBA_v3(videoFilePath, threshold1, threshold2, logFile);
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFile[MAX_PATH], outputFileName[MAX_PATH];
				int index = f.first;
				strcpy(outputFile, outputDirPaths[2]);
				strcat(outputFile, genericFileName);
				sprintf(outputFileName, outputFile, index);

				// save the current frame
				imwrite(outputFileName, f.second);
			}
			waitKey(0);
			break;
		}
		case 4: {
			float M,N;

			cout << "Bigger sliding window size = ";
			cin >> M;

			cout << "Smaller sliding window size = ";
			cin >> N;

			fs::remove_all(outputDirPaths[3]);
			fs::create_directory(outputDirPaths[3]);

			vector<pair<int, Mat>> frames = PBA_v4(videoFilePath, M, N, logFile);
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFile[MAX_PATH], outputFileName[MAX_PATH];
				int index = f.first;
				strcpy(outputFile, outputDirPaths[3]);
				strcat(outputFile, genericFileName);
				sprintf(outputFileName, outputFile, index);

				// save the current frame
				imwrite(outputFileName, f.second);
			}
			waitKey(0);
			break;
		}
		case 5: {
			// todo: change it to use TH and TL thresholds! (HD > TH => cut, TL < HD < TH => ST)

			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			fs::remove_all(outputDirPaths[4]);
			fs::create_directory(outputDirPaths[4]);

			vector<pair<int, Mat>> frames = HBA(videoFilePath, threshold, logFile, BIN_TO_BIN_DIFFERENCE);
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFile[MAX_PATH], outputFileName[MAX_PATH];
				int index = f.first;
				strcpy(outputFile, outputDirPaths[4]);
				strcat(outputFile, genericFileName);
				sprintf(outputFileName, outputFile, index);

				// save the current frame
				imwrite(outputFileName, f.second);
			}
			waitKey(0);
			break;
		}
		case 6: {
			// todo: change it to use TH and TL thresholds! (HD > TH => cut, TL < HD < TH => ST)

			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			fs::remove_all(outputDirPaths[5]);
			fs::create_directory(outputDirPaths[5]);

			vector<pair<int, Mat>> frames = HBA(videoFilePath, threshold, logFile, CHI_SQUARE_TEST);
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFile[MAX_PATH], outputFileName[MAX_PATH];
				int index = f.first;
				strcpy(outputFile, outputDirPaths[5]);
				strcat(outputFile, genericFileName);
				sprintf(outputFileName, outputFile, index);

				// save the current frame
				imwrite(outputFileName, f.second);
			}
			waitKey(0);
			break;
		}
		case 7: {
			// todo: change it to use TH and TL thresholds! (HD > TH => cut, TL < HD < TH => ST)

			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			fs::remove_all(outputDirPaths[6]);
			fs::create_directory(outputDirPaths[6]);

			vector<pair<int, Mat>> frames = HBA(videoFilePath, threshold, logFile, HIST_INTERSECTION);
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFile[MAX_PATH], outputFileName[MAX_PATH];
				int index = f.first;
				strcpy(outputFile, outputDirPaths[6]);
				strcat(outputFile, genericFileName);
				sprintf(outputFileName, outputFile, index);

				// save the current frame
				imwrite(outputFileName, f.second);
			}
			waitKey(0);
			break;
		}
		default:break;
		}
	} while (op != 0);

	logFile.close();
	return 0;
}
