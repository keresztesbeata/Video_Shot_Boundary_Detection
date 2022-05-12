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

void saveKeyFrames(vector<Shot> keyFrames, char* outputDirPath) {
	char genericFileName[] = "/%s_shot_%d.jpg";

	// clean and remove the output directory
	fs::remove_all(outputDirPath);
	// create a new output directory to store the detected keyframes
	fs::create_directory(outputDirPath);

	for (auto f : keyFrames) {
		// compute the output path for the current frame
		char outputFile[MAX_PATH], outputFileName[MAX_PATH];
		int index = f.index;
		strcpy(outputFile, outputDirPath);
		strcat(outputFile, genericFileName);
		sprintf(outputFileName, outputFile, transitionToString(f.type), index);

		// save the current frame
		imwrite(outputFileName, f.keyFrame);
	}
}


void saveTransitionFrames(vector<GradualTransition> gradTransitions, vector<Mat> allFrames, char* outputDirPath) {
	char genericFileName[] = "/%s_frame_%d.jpg";
	
	for (auto gt : gradTransitions) {
		int index = gt.start;
		const char* effect = effectToString(gt.effect);
		while (index <= gt.end) {
			// compute the output path for the current frame transitions
			char outputFile[MAX_PATH], outputFileName[MAX_PATH];

			strcpy(outputFile, outputDirPath);
			strcat(outputFile, genericFileName);
			sprintf(outputFileName, outputFile, effect, index);
			// save the current frame
			imwrite(outputFileName, allFrames[index]);
			index++;
		}
	}
}