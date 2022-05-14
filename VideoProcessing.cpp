#include "stdafx.h"
#include "VideoProcessing.h"
#include "GradualTransitionDetector.h"

namespace fs = std::filesystem;

vector<Mat> readAllFrames(const char* fileName) {
	vector<Mat> frames;
	Mat currentFrame;

	// open video file for reading
	VideoCapture videoCapture(fileName);
	if (!videoCapture.isOpened()) {
		cerr << "Cannot open video file!" << endl;
	}
	int i = 0;
	while (videoCapture.read(currentFrame)) {
		frames.push_back(currentFrame.clone());
	}
	return frames;
}

void saveAllFrames(vector<Mat> allFrames, char* videoFilePath, char* parentDirPath) {
	char outputDirPath[MAX_PATH];
	strcpy(outputDirPath, parentDirPath);
	strcat(outputDirPath, "/all");

	fs::remove_all(outputDirPath);
	fs::create_directory(outputDirPath);

	int n = allFrames.size();
	for (int i = 0; i < n; i++) {
		char outputFile[MAX_PATH];
		strcpy(outputFile, outputDirPath);
		strcat(outputFile,"/frame_%ld.jpg");
		sprintf(outputFile, outputFile, i);

		imwrite(outputFile, allFrames[i]);
	}
}


void saveKeyFrames(vector<FrameTransition> keyFrames, vector<Mat> allFrames, char* outputDirPath) {
	char genericFileName[] = "/kf#%ld_%s_%ld.jpg";

	int totalNrFrames = allFrames.size();

	int N = keyFrames.size();
	for (int n = 0; n < N; n++) {
		const char* type = transitionToString(keyFrames[n].type);
		if (keyFrames[n].type == CUT) {
			char outputFile[MAX_PATH], outputFileName[MAX_PATH];
			strcpy(outputFile, outputDirPath);
			strcat(outputFile, genericFileName);
			sprintf(outputFileName, outputFile, n, type, keyFrames[n].start);
			// save the current frame
			imwrite(outputFileName, allFrames[keyFrames[n].start]);
		}
		else {
			int i = keyFrames[n].start;
			while (i <= keyFrames[n].end && i < totalNrFrames) {
				// compute the output path for the current frame transitions
				char outputFile[MAX_PATH], outputFileName[MAX_PATH];

				strcpy(outputFile, outputDirPath);
				strcat(outputFile, genericFileName);
				sprintf(outputFileName, outputFile, n, type, i);
				// save the current frame
				imwrite(outputFileName, allFrames[i]);
				i++;
			}
		}
	}
}