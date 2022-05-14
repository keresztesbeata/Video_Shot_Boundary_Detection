#include "stdafx.h"
#include "MBAalgorithms.h"


float TSS(Mat currentFrame, Mat prevFrame, int N) {
	int height = currentFrame.rows;
	int width = currentFrame.cols;

	// start with search loc at center
	int cx = height / 2;
	int cy = width / 2;

	int nrNeighb = 9; // nr of neighbour pixels searched 
	int dx[] = { -1,-1,-1,0,0,0,1,1,1 };
	int dy[] = { -1,0,1,-1,0,1,-1,0,1 };

	int S = 4; // step size / search perimeter

	float minCost = INT_MAX;

	while (S > 0) {
		int bx = cx, by = cy; // best point (with min cost)
		// search the 9 locations around the center point and pick the one with the min cost
		for (int i = 0; i < nrNeighb; i++) {
			if (isInside(currentFrame, cx + S*dx[i], cy + S*dy[i])) {
				float mse = MSE(currentFrame, prevFrame, cx, cy, N);
				if (mse < minCost) {
					minCost = mse;
					bx = cx + dx[i];
					by = cy + dy[i];
				}
			}
		}
		// set the new search origin to the loc with min cost
		cx = bx;
		cy = by;

		// set the new step size
		S /= 2;
	}

	// the resulting loc for S=1 is the min cost function and this macro block is the best match
	return minCost;
}

Mat_<Vec3b> extractBlock(Mat img, int N, int cx, int cy) {
	Mat_<Vec3b> roi(N, N);
	int w = N / 2;
	for (int i = 0; i < 2*w; i++) {
		for (int j = 0; j < 2*w; j++) {
			if (isInside(img, cx - w + i, cy - w + j)) {
				roi(i, j) = img.at<Vec3b>(cx - w + i, cy - w + j);
			}
		}
	}
	return roi;
}

float MSE(Mat currentFrame, Mat prevFrame, int cx, int cy, int N) {
	int w = N / 2;
	float sum = 0;
	for (int i = cx - w; i <= cx + w; i++) {
		for (int j = 0; j <= cy + w; j++) {
			if (isInside(currentFrame, i, j)) {
				int diff = getPixelDifference(currentFrame.at<Vec3b>(i, j), prevFrame.at<Vec3b>(i, j));
				sum += diff * diff;
			}
		}
	}
	return (float)sum / (float)(N * N);
}

int getPixelDifference(Vec3b c, Vec3b p) {
	return abs(c[2] - p[2]) + abs(c[1] - p[1]) + abs(c[0] - p[0]);
}

int selectShot_BMA(Mat* frames, int start, int end, int N) {
	float maxDiff = 0;
	float maxIdx = start;
	for (int i = start; i < end; i++) {
		float mse = TSS(frames[i], frames[i + 1], N);
		if (mse > maxDiff) {
			maxDiff = mse;
			maxIdx = i+1;
		}
	}
	return maxIdx;
}

vector<FrameTransition> MBA(const char* fileName, float T, int N, int M, int B, ofstream& logFile) {
	vector<FrameTransition> keyFrames;
	vector<float> difference;
	Mat previousFrame, currentFrame;
	vector<Mat> frames;

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

	frames.push_back(previousFrame.clone());

	while (videoCapture.read(currentFrame))
	{
		frames.push_back(currentFrame.clone());

		float cost = TSS(previousFrame, currentFrame, B);
		difference.push_back(cost);

		previousFrame = currentFrame.clone();
	}

	int nrFrames = frames.size();

	for (int n = M; n < nrFrames - M; n++) {
		float meanLeft = getMeanOfRegion(difference, n - M, n);
		float varianceLeft = getVarianceOfRegion(difference, n - M, n, meanLeft);

		float meanRight = getMeanOfRegion(difference, n + 1, n + M);
		float varianceRight = getVarianceOfRegion(difference, n + 1, n + M, meanRight);

		float TH = max(meanLeft + T * sqrt(varianceLeft), meanRight + T * sqrt(varianceRight));
		float TL = TH / 5.0;

		float diff = difference[n];

		float max = getMaxOfRegion(difference, n - N, n + N);

		if (diff >= max && diff > TH) {
			// abrupt shot boundary
			FrameTransition shot = { n-1, n, CUT };
			keyFrames.push_back(shot);
			logFile << "keyFrame #" << n << endl;
		}
		else if (diff > TL) {
			// gradual shot boundary
			FrameTransition shot = { n-1, n, GRADUAL };
			keyFrames.push_back(shot);
			logFile << "keyFrame #" << n << endl;
		}
	}

	logFile << " -> MBA (bma):";
	logFile << " T = " << T << endl;
	logFile << "	Nr of all frames / nr of key frames (" << nrFrames << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}