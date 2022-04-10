#include "stdafx.h"
#include "CutDetectionAlgorithms.h"

vector<pair<int, Mat>> pixelBasedApproach(const char * fileName, float threshold, ofstream& logFile) {
	vector<pair<int, Mat>> keyFrames;
	Mat previousFrame, currentFrame;
	int nrFrames = 0;

	// open video file for reading
	VideoCapture videoCapture(fileName); 
	if (!videoCapture.isOpened()) {
		cerr<<"Cannot open video file!"<<endl;
		goto FINISH;
	}

	if (!videoCapture.read(previousFrame)) {
		cerr << "No frames contained in the video!"<<endl;
		goto FINISH;
	}

	// get the nr of channels to determine if it is a grayscale/image video
	int nrChannels = previousFrame.channels();

	while (videoCapture.read(currentFrame))
	{

		float d = 
			(nrChannels == 1)? 
			getDissimilarityFactorForGrayScale(previousFrame, currentFrame)
			: 
			getDissimilarityFactorForColour(previousFrame, currentFrame)
			;

		if (d > threshold) {
			keyFrames.push_back(make_pair(nrFrames,previousFrame));
			logFile << "Key frame #" << nrFrames << endl;
		}

		previousFrame = currentFrame.clone();
		
		nrFrames++;
	}
	logFile << " -> PBA with single threshold: T = " << threshold << endl;
	logFile << "	Nr of all frames / nr of key frames (" << nrFrames << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}

float getDissimilarityFactorForGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame) {
	int height = previousFrame.rows;
	int width = previousFrame.cols;

	float sum = 0;
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			sum += abs(previousFrame(x, y) - currentFrame(x, y));
		}
	}

	return sum / (float)(height * width);
}

float getDissimilarityFactorForColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame) {
	int height = previousFrame.rows;
	int width = previousFrame.cols;

	// extract the 3 colour channels: R, G, B from both previous and current frames
	Mat_<uchar> r_prev(height, width);
	Mat_<uchar> g_prev(height, width);
	Mat_<uchar> b_prev(height, width);
	extractRGBChannelsFromColourImage(previousFrame, r_prev, g_prev, b_prev);

	Mat_<uchar> r_curr(height, width);
	Mat_<uchar> g_curr(height, width);
	Mat_<uchar> b_curr(height, width);
	extractRGBChannelsFromColourImage(currentFrame, r_curr, g_curr, b_curr);

	float d_r = getDissimilarityFactorForGrayScale(r_prev, r_curr);
	float d_g = getDissimilarityFactorForGrayScale(g_prev, g_curr);
	float d_b = getDissimilarityFactorForGrayScale(b_prev, b_curr);

	return d_r + d_g + d_b;
}

vector<pair<int, Mat>> pixelBasedApproachWithMultipleThresholds(const char* fileName, float threshold1, float threshold2, ofstream& logFile) {
	vector<pair<int, Mat>> keyFrames;
	Mat previousFrame, currentFrame;
	int nrFrames = 0;

	// open video file for reading
	VideoCapture videoCapture(fileName);
	if (!videoCapture.isOpened()) {
		cerr << "Cannot open video file!" << endl;
		goto FINISH;
	}

	if (!videoCapture.read(previousFrame)) {
		cerr << "No frames contained in the video!" << endl;
		goto FINISH;
	}

	// get the nr of channels to determine if it is a grayscale/image video
	int nrChannels = previousFrame.channels();

	while (videoCapture.read(currentFrame))
	{

		float d =
			(nrChannels == 1) ?
			getDissimilarityCountForGrayScale(previousFrame, currentFrame, threshold1)
			:
			getDissimilarityCountForColour(previousFrame, currentFrame, threshold1)
			;

		if (d > threshold2) {
			keyFrames.push_back(make_pair(nrFrames,previousFrame));
			logFile << "Key frame #" << nrFrames << endl;
		}

		previousFrame = currentFrame.clone();

		nrFrames++;
	}

	logFile << " -> PBA with multiple thresholds: " << endl;
	logFile << "	T1 = " << threshold1 << endl;
	logFile << "	T2 = " << threshold2 << endl;
	logFile << "Nr of all frames / nr of key frames (" << nrFrames << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}

float getDissimilarityCountForGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, float threshold) {
	int height = previousFrame.rows;
	int width = previousFrame.cols;

	int count = 0;
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			if (abs(previousFrame(x, y) - currentFrame(x, y)) > threshold) {
				count++;
			}
		}
	}

	return count / (float)(height * width);
}

float getDissimilarityCountForColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame, float threshold) {
	int height = previousFrame.rows;
	int width = previousFrame.cols;

	// extract the 3 colour channels: R, G, B from both previous and current frames
	Mat_<uchar> r_prev(height, width);
	Mat_<uchar> g_prev(height, width);
	Mat_<uchar> b_prev(height, width);
	extractRGBChannelsFromColourImage(previousFrame, r_prev, g_prev, b_prev);

	Mat_<uchar> r_curr(height, width);
	Mat_<uchar> g_curr(height, width);
	Mat_<uchar> b_curr(height, width);
	extractRGBChannelsFromColourImage(currentFrame, r_curr, g_curr, b_curr);

	float d_r = getDissimilarityCountForGrayScale(r_prev, r_curr, threshold);
	float d_g = getDissimilarityCountForGrayScale(g_prev, g_curr, threshold);
	float d_b = getDissimilarityCountForGrayScale(b_prev, b_curr, threshold);

	return d_r + d_g + d_b;
}

Mat_<uchar> applyAveragingFilterOnGrayScaleImage(Mat_<uchar> src) {
	int height = src.rows;
	int width = src.cols;

	Mat_<uchar> dst(height, width);

	int dx[] = { -1,-1,-1,0,0,1,1,1 };
	int dy[] = { -1,0,1,-1,1,-1,0,1 };
	int n = sizeof(dx) / sizeof(int);

	for (int i = 1; i < height-1; i++) {
		for (int j = 1; j < width-1; j++) {
			float average = 0;
			for (int k = 0; k < n; k++) {
				average += src(i + dx[k], j + dy[k]);
			}
			dst(i, j) = average / (float)n;
		}
	}

	return dst;
}

Mat_<Vec3b> applyAveragingFilterOnColourImage(Mat_<Vec3b> src) {
	int height = src.rows;
	int width = src.cols;

	Mat_<Vec3b> dst(height, width);

	int dx[] = { -1,-1,-1,0,0,1,1,1 };
	int dy[] = { -1,0,1,-1,1,-1,0,1 };
	int n = sizeof(dx) / sizeof(int);

	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			Vec3b average = { 0,0,0 };
			for (int k = 0; k < n; k++) {
				average += src(i + dx[k], j + dy[k]);
			}
			dst(i, j) = average / (float)n;
		}
	}

	return dst;
}

vector<pair<int, Mat>> pixelBasedApproachWithMultipleThresholdsAndNoiseFiltering(const char* fileName, float threshold1, float threshold2, ofstream& logFile) {
	vector<pair<int, Mat>> keyFrames;
	Mat previousFrame, currentFrame;
	int nrFrames = 0;

	// open video file for reading
	VideoCapture videoCapture(fileName);
	if (!videoCapture.isOpened()) {
		cerr << "Cannot open video file!" << endl;
		goto FINISH;
	}

	if (!videoCapture.read(previousFrame)) {
		cerr << "No frames contained in the video!" << endl;
		goto FINISH;
	}

	// get the nr of channels to determine if it is a grayscale/image video
	int nrChannels = previousFrame.channels();

	while (videoCapture.read(currentFrame))
	{
		float d = 0;

		if (nrChannels == 1) {
			// grayscale video

			// pre-processing step on frames beofre comparing them:
			Mat_<uchar> filteredPrevFrame = applyAveragingFilterOnGrayScaleImage((Mat_<uchar>)previousFrame);
			Mat_<uchar> filteredCurrFrame = applyAveragingFilterOnGrayScaleImage((Mat_<uchar>)currentFrame);

			d = getDissimilarityCountForGrayScale(filteredPrevFrame, filteredCurrFrame, threshold1);
		}
		else {
			// colour video

			// pre-processing step on frames beofre comparing them:
			Mat_<Vec3b> filteredPrevFrame = applyAveragingFilterOnColourImage((Mat_<Vec3b>)previousFrame);
			Mat_<Vec3b> filteredCurrFrame = applyAveragingFilterOnColourImage((Mat_<Vec3b>)currentFrame);

			d = getDissimilarityCountForColour(filteredPrevFrame, filteredCurrFrame, threshold1);
		}

		if (d > threshold2) {
			keyFrames.push_back(make_pair(nrFrames, previousFrame));
			logFile << "Key frame #" << nrFrames << endl;
		}

		previousFrame = currentFrame.clone();

		nrFrames++;
	}

	logFile << " -> PBA with multiple thresholds and noise filtering: " << endl;
	logFile << "	T1 = " << threshold1 << endl;
	logFile << "	T2 = " << threshold2 << endl;
	logFile << "Nr of all frames / nr of key frames (" << nrFrames << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}
