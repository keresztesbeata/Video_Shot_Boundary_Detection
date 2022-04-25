#include "stdafx.h"
#include "EBAalgorithms.h"

float ECR(Mat previousFrame, Mat currentFrame) {
	int height = previousFrame.rows;
	int width = previousFrame.cols;

	Mat prevEdges, currEdges;

	prevEdges = cannyEdgeDetector(previousFrame);
	currEdges = cannyEdgeDetector(currentFrame);
	
	// max distance between edge pixels of 2 consecutive frames considered as part of the same edge
	float r = 5;

	// count total edge pixels in prev and current frames
	int prevTotalEdgeCount = 1;
	int currTotalEdgeCount = 1;
	// ocunt only the incoming edge pixels: missing from previous frame, but present in the current frame
	int incomingEdgeCount = 0;
	// ocunt only the outgoing edge pixels: present in the previous frame, but missing from current frame
	int outgoingEdgeCount = 0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			bool prev = isEdgePixel(prevEdges, i, j);
			bool curr = isEdgePixel(currEdges, i, j);
			if (prev) {
				prevTotalEdgeCount++;
			}
			if (curr) {
				currTotalEdgeCount++;
			}
			
			if (!prev && curr) {
				incomingEdgeCount++;
			}
			if (prev && !curr) {
				outgoingEdgeCount++;
			}
			/*
			if (!curr) {
				float dist = getDistanceToClosestEdgePixel(currEdges, i, j, 10);
				if (!prev && dist < r) {
					incomingEdgeCount++;
				}
				if (prev && dist > r) {
					outgoingEdgeCount++;
				}
			}
			*/
		}
	}
	/*
	cout << "total prev = " << prevTotalEdgeCount << endl;
	cout << "total curr = " << currTotalEdgeCount << endl;
	cout << "incoming = " << incomingEdgeCount << endl;
	cout << "outgoing = " << outgoingEdgeCount << endl;
	*/

	float ecr_in = (float)incomingEdgeCount / (float)currTotalEdgeCount;
	float ecr_out = (float)outgoingEdgeCount / (float)prevTotalEdgeCount;
	float ecr = max(ecr_in, ecr_out);

	return ecr;
}

float getDistanceToClosestEdgePixel(Mat img, int x, int y, int w) {
	int height = img.rows;
	int width = img.cols;
	float minDist = height + width;
	for (int i = x - w; i >= 0 && i <= x + w && i < height; i++) {
		for (int j = y - w; j >= 0 && j <= y + w && j < width; j++) {
			if ((i != x || j != y) && isEdgePixel(img, i, j)) {
				float dist = sqrt((x - i) * (x - i) + (y - j) * (y - j));
				if (dist < minDist) {
					minDist = dist;
				}
			}
		}
	}
	return minDist;
}

vector<pair<int, Mat>> EBA_v1(const char* fileName, float T, int N, int M, ofstream& logFile) {
	vector<pair<int, Mat>> keyFrames;
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

		float ecr = ECR(previousFrame, currentFrame);
		difference.push_back(ecr);

		previousFrame = currentFrame.clone();
	}
	
	int nrFrames = frames.size();

	for (int n = M; n < nrFrames - M; n++) {
		float meanLeft = getMeanOfRegion(difference, n - M, n);
		float varianceLeft = getVarianceOfRegion(difference, n - M, n, meanLeft);

		float meanRight = getMeanOfRegion(difference, n + 1, n + M);
		float varianceRight = getVarianceOfRegion(difference, n + 1, n + M, meanRight);

		float Ts = max(meanLeft + T * sqrt(varianceLeft), meanRight + T * sqrt(varianceRight));
		float ecr = difference[n];

		float max = getMaxOfRegion(difference, n - N, n + N);

		if (ecr >= max && ecr > Ts) {
			// cut shot boundary
			keyFrames.push_back(make_pair(n, frames[n]));
			logFile << "keyFrame #" << n << endl;
		}
	}

	logFile << " -> EBA_v1 (ecr):";
	logFile << " T = " << T << endl;
	logFile << "	Nr of all frames / nr of key frames (" << nrFrames << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}


float getMeanOfRegion(vector<float> difference, int start, int end) {
	float mean = 0;
	for (int i = start; i < end; i++) {
		mean += difference[i];
	}

	return mean / (float)(end-start);
}

float getVarianceOfRegion(vector<float> difference, int start, int end, float mean) {
	float variance = 0;
	int total = end - start;
	for (int i = start; i < end; i++) {
		variance += (difference[i] - mean) * (difference[i] - mean);
	}

	return mean / (float)total;
}

float getMaxOfRegion(vector<float> difference, int start, int end) {
	float max = 0;
	for (int i = start; i < end; i++) {
		if (difference[i] > max) {
			max = difference[i];
		}
	}

	return max;
}