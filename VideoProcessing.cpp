#include "stdafx.h"
#include "CutDetectionAlgorithms.h"
#include <fstream>
#include <ctime>

const char videoFilePath[] = "Videos/Megamind.avi";
const char outputDirPaths[][MAX_PATH] = { "Videos/keyFrames/PBA_1/key_frame_%d.jpg" , "Videos/keyFrames/PBA_2/key_frame_%d.jpg", "Videos/keyFrames/PBA_3/key_frame_%d.jpg" };

const char logFilePath[] = "logs/pba_logs.txt";

int main() {
	int op;
	ofstream logFile(logFilePath, ofstream::out | ofstream::app);
	time_t now = time(0);
	logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	logFile << "<<<< " << ctime(&now);
	logFile << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;

	do
	{
		system("cls");
		destroyAllWindows();
		cout << "Menu:" << endl;
		cout << " 1 - Cut detection using PBA and single custom threshold" << endl;
		cout << " 2 - Cut detection using PBA and multiple thresholds" << endl;
		cout << " 3 - Cut detection using PBA, multiple thresholds and noise filtering" << endl;
		cout << " 0 - Exit" << endl;
		cout << "Option: " << endl;
		cin >> op;
		switch (op)
		{
		case 1: {
			float threshold;
			cout << "threshold = ";
			cin >> threshold;

			vector<pair<int, Mat>> frames = pixelBasedApproach(videoFilePath, threshold, logFile);
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFileName[MAX_PATH];
				int index = f.first;
				sprintf(outputFileName, outputDirPaths[0], index);
				// save the current frame
				//imwrite(outputFileName, f.second);
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

			vector<pair<int, Mat>> frames = pixelBasedApproachWithMultipleThresholds(videoFilePath, threshold1, threshold2, logFile);
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFileName[MAX_PATH];
				int index = f.first;
				sprintf(outputFileName, outputDirPaths[1], index);
				// save the current frame
				//imwrite(outputFileName, f.second);
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

			vector<pair<int, Mat>> frames = pixelBasedApproachWithMultipleThresholdsAndNoiseFiltering(videoFilePath, threshold1, threshold2, logFile);
			for (auto f : frames) {
				// compute the output path for the current frame
				char outputFileName[MAX_PATH];
				int index = f.first;
				sprintf(outputFileName, outputDirPaths[2], index);
				// save the current frame
				//imwrite(outputFileName, f.second);
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
