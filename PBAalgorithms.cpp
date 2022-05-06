#include "stdafx.h"
#include "PBAalgorithms.h"
#include "Utils.h"

vector<Shot> PBA_v1(const char * fileName, float T, ofstream& logFile) {
	vector<Shot> keyFrames;
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
			getDFGrayScale(previousFrame, currentFrame)
			: 
			getDFColour(previousFrame, currentFrame)
			;

		if (d > T) {
			Shot shot = { nrFrames,previousFrame, HARD_CUT };
			keyFrames.push_back(shot);
			logFile << "Key frame #" << nrFrames << endl;
		}

		previousFrame = currentFrame.clone();
		
		nrFrames++;
	}
	logFile << " -> PBA with single threshold: T = " << T << endl;
	logFile << "	Nr of all frames / nr of key frames (" << nrFrames << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}

float getDFGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame) {
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

float getDFColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame) {
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

	float d_r = getDFGrayScale(r_prev, r_curr);
	float d_g = getDFGrayScale(g_prev, g_curr);
	float d_b = getDFGrayScale(b_prev, b_curr);

	return d_r + d_g + d_b;
}

vector<Shot> PBA_v2(const char* fileName, float T1, float T2, ofstream& logFile) {
	vector<Shot> keyFrames;
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

	// get the nr of channels to determine if it is a grayscale/colour video
	int nrChannels = previousFrame.channels();

	while (videoCapture.read(currentFrame))
	{

		float d =
			(nrChannels == 1) ?
			getDCGrayScale(previousFrame, currentFrame, T1)
			:
			getDCColour(previousFrame, currentFrame, T1)
			;

		if (d > T2) {
			Shot shot = { nrFrames, previousFrame, HARD_CUT };
			keyFrames.push_back(shot);
			logFile << "Key frame #" << nrFrames << endl;
		}

		previousFrame = currentFrame.clone();

		nrFrames++;
	}

	logFile << " -> PBA with multiple thresholds: " << endl;
	logFile << "	T1 = " << T1 << endl;
	logFile << "	T2 = " << T2 << endl;
	logFile << "Nr of all frames / nr of key frames (" << nrFrames << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}

float getDCGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, float T) {
	int height = previousFrame.rows;
	int width = previousFrame.cols;

	int count = 0;
	for (int x = 0; x < height; x++) {
		for (int y = 0; y < width; y++) {
			if (abs(previousFrame(x, y) - currentFrame(x, y)) > T) {
				count++;
			}
		}
	}

	return count / (float)(height * width);
}

float getDCColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame, float T) {
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

	float d_r = getDCGrayScale(r_prev, r_curr, T);
	float d_g = getDCGrayScale(g_prev, g_curr, T);
	float d_b = getDCGrayScale(b_prev, b_curr, T);

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
			float r = 0, g = 0, b = 0;
			for (int k = 0; k < n; k++) {
				r += src(i + dx[k], j + dy[k])[2];
				g += src(i + dx[k], j + dy[k])[1];
				b += src(i + dx[k], j + dy[k])[0];
			}
			dst(i, j)[2] = r / n;
			dst(i, j)[1] = g / n;
			dst(i, j)[0] = b / n;
		}
	}

	return dst;
}

vector<Shot> PBA_v3(const char* fileName, float T1, float T2, ofstream& logFile) {
	vector<Shot> keyFrames;
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

	// get the nr of channels to determine if it is a grayscale/colour video
	int nrChannels = previousFrame.channels();

	while (videoCapture.read(currentFrame))
	{
		float d = 0;
		Mat filteredPrevFrame, filteredCurrFrame;
		if (nrChannels == 1) {
			// grayscale video

			// pre-processing step on frames beofre comparing them:
			filteredPrevFrame = applyAveragingFilterOnGrayScaleImage((Mat_<uchar>)previousFrame);
			filteredCurrFrame = applyAveragingFilterOnGrayScaleImage((Mat_<uchar>)currentFrame);

			d = getDCGrayScale(filteredPrevFrame, filteredCurrFrame, T1);
		}
		else {
			// colour video

			// pre-processing step on frames beofre comparing them:
			filteredPrevFrame = applyAveragingFilterOnColourImage((Mat_<Vec3b>)previousFrame);
			filteredCurrFrame = applyAveragingFilterOnColourImage((Mat_<Vec3b>)currentFrame);

			d = getDCColour(filteredPrevFrame, filteredCurrFrame, T1);
		}

		if (d > T2) {
			Shot shot = { nrFrames,previousFrame, HARD_CUT };
			keyFrames.push_back(shot);
			logFile << "Key frame #" << nrFrames << endl;
		}

		previousFrame = currentFrame.clone();

		nrFrames++;
	}

	logFile << " -> PBA with multiple thresholds and noise filtering: " << endl;
	logFile << "	T1 = " << T1 << endl;
	logFile << "	T2 = " << T2 << endl;
	logFile << "Nr of all frames / nr of key frames (" << nrFrames << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}

vector<Shot> PBA_v4(const char* fileName, int M, int N, ofstream& logFile) {
	vector<Shot> keyFrames;
	vector<float> difference;
	Mat previousFrame, currentFrame;
	int n = 0;

	logFile << "---------------------------" << endl;
	logFile << "	M = " << M << endl;
	logFile << "	N = " << N << endl;
	logFile << "---------------------------" << endl;

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

	// get the nr of channels to determine if it is a grayscale/colour video
	int nrChannels = previousFrame.channels();

	logFile << " -> PBA with adaptive thresholding: " << endl;

	while (videoCapture.read(currentFrame))
	{
		float d = 0;

		if (nrChannels == 1) {
			// grayscale video

			// pre-processing step on frames beofre comparing them:
			Mat_<uchar> filteredPrevFrame = applyAveragingFilterOnGrayScaleImage((Mat_<uchar>)previousFrame);
			Mat_<uchar> filteredCurrFrame = applyAveragingFilterOnGrayScaleImage((Mat_<uchar>)currentFrame);

			d = getDFGrayScale(filteredPrevFrame, filteredCurrFrame);
		}
		else {
			// colour video

			// pre-processing step on frames beofre comparing them:
			Mat_<Vec3b> filteredPrevFrame = applyAveragingFilterOnColourImage((Mat_<Vec3b>)previousFrame);
			Mat_<Vec3b> filteredCurrFrame = applyAveragingFilterOnColourImage((Mat_<Vec3b>)currentFrame);

			d = getDFColour(filteredPrevFrame, filteredCurrFrame);
		}

		difference.push_back(d);

		float mean = getAverageDFFromSlidingWindow(difference, n, M);

		float T1 = 5 * mean;
		float T2 = 1.5 * mean;

		float max = getMaxDFFromSlidingWindow(difference, n, N);

		if (max > T1) {
			// cut shot boundary
			Shot shot = { n,previousFrame, HARD_CUT };
			keyFrames.push_back(shot);
			logFile << "HT (cut) shot #" << n << endl;
		}
		else if(max > T2){
			// start of a frame or the middle of a gradual transition
			Shot shot = { n,previousFrame, SOFT_CUT };
			keyFrames.push_back(shot);
			logFile << "ST (gradual transition) shot #" << n << endl;
		}

		previousFrame = currentFrame.clone();

		n++;
	}
	logFile << "Nr of all frames / nr of key frames (" << n << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}


float getAverageDFFromSlidingWindow(vector<float> difference, int n, int windowSize) {
	float mean = 0;
	for (int i = n- windowSize; i < n; i++) {
		mean += difference[i];
	}

	return mean / (float)windowSize;
}

float getMaxDFFromSlidingWindow(vector<float> difference, int n, int windowSize) {
	float max = 0;
	for (int i = n - windowSize; i < n; i++) {
		if (difference[i] > max) {
			max = difference[i];
		}
	}

	return max;
}