#include "stdafx.h"
#include "VideoProcessing.h"
#include "GradualTransitionDetector.h"

namespace fs = std::filesystem;

vector<Mat> readAllFrames(string fileName) {
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

void saveAllFrames(vector<Mat> allFrames, string parentDirPath) {
	string outputDirPath = parentDirPath + "/all";

	fs::remove_all(outputDirPath);
	fs::create_directory(outputDirPath);

	int n = allFrames.size();
	for (int i = 0; i < n; i++) {
		string outputFilePath = outputDirPath + "/frame_" + to_string(i) + ".jpg";
		
		imwrite(outputFilePath, allFrames[i]);
	}
}


void saveKeyFrames(vector<FrameTransition> keyFrames, vector<Mat> allFrames, string outputDirPath) {
	int totalNrFrames = allFrames.size();

	int N = keyFrames.size();
	for (int n = 0; n < N; n++) {
		const char* type = transitionToString(keyFrames[n].type);
		if (keyFrames[n].type == CUT) {
			string outputFilePath = outputDirPath + "/kf#"+ to_string(n) + "_" + transitionToString(keyFrames[n].type) + "_" + to_string(keyFrames[n].end) + ".jpg";
			// save the current frame
			imwrite(outputFilePath, allFrames[keyFrames[n].end]);
		}
		else {
			int i = keyFrames[n].start;
			while (i <= keyFrames[n].end && i < totalNrFrames) {
				// compute the output path for the current frame transitions
				string outputFilePath = outputDirPath + "/kf#" + to_string(n) + "_" + transitionToString(keyFrames[n].type) + "_" + to_string(i) + ".jpg";
				// save the current frame
				imwrite(outputFilePath, allFrames[i]);
				i++;
			}
		}
	}
}