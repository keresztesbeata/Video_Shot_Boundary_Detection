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

void saveAllFrames(vector<Mat> allFrames, char* videoFilePath, char* outputDirPath) {
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
	
	int nr = 0;
	for (auto kf : keyFrames) {
		int i = kf.start;
		const char* type = transitionToString(kf.type);
		while (i <= kf.end) {
			// compute the output path for the current frame transitions
			char outputFile[MAX_PATH], outputFileName[MAX_PATH];

			strcpy(outputFile, outputDirPath);
			strcat(outputFile, genericFileName);
			sprintf(outputFileName, outputFile, nr, type, i);
			// save the current frame
			imwrite(outputFileName, allFrames[i]);
			i++;
		}
		nr++;
	}
}