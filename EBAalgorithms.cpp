#include "stdafx.h"
#include "EBAalgorithms.h"


void computeECR(Mat previousFrame, Mat currentFrame, float& ecr_in, float& ecr_out) {
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
	// count only the incoming edge pixels: missing from previous frame, but present in the current frame
	int incomingEdgeCount = 0;
	// count only the outgoing edge pixels: present in the previous frame, but missing from current frame
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

			if (curr && !isPointInNeighbourhood(previousFrame, i, j, r)) {
				incomingEdgeCount++;
			}
			if (prev && !isPointInNeighbourhood(currentFrame, i, j, r)) {
				outgoingEdgeCount++;
			}
		}
	}

	ecr_in = (float)incomingEdgeCount / (float)currTotalEdgeCount;
	ecr_out = (float)outgoingEdgeCount / (float)prevTotalEdgeCount;
}

float ECR(Mat previousFrame, Mat currentFrame) {
	int height = previousFrame.rows;
	int width = previousFrame.cols;

	Mat prevEdges, currEdges;

	prevEdges = cannyEdgeDetector(previousFrame);
	currEdges = cannyEdgeDetector(currentFrame);
	
	// max distance between edge pixels of 2 consecutive frames considered as part of the same edge
	float r = 1;

	// count total edge pixels in prev and current frames
	int prevTotalEdgeCount = 1;
	int currTotalEdgeCount = 1;
	// count only the incoming edge pixels: missing from previous frame, but present in the current frame
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
			
			if (curr && !isPointInNeighbourhood(previousFrame, i, j, r)) {
				incomingEdgeCount++;
			}
			if (prev && !isPointInNeighbourhood(currentFrame, i, j, r)) {
				outgoingEdgeCount++;
			}
		}
	}

	float ecr_in = (float)incomingEdgeCount / (float)currTotalEdgeCount;
	float ecr_out = (float)outgoingEdgeCount / (float)prevTotalEdgeCount;
	float ecr = max(ecr_in, ecr_out);

	return ecr;
}

bool isPointInNeighbourhood(Mat img, int x, int y, int r) {
	for (int i = x - r; i <= x + r; i++) {
		for (int j = y - r; j <= y + r; j++) {
			if (isInside(img, i, j) && isEdgePixel(img, i, j)) {
				return true;
			}
		}
	}
	return false;
}

vector<FrameTransition> EBA(const char* fileName, float T, int N, int M, ofstream& logFile) {
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
			FrameTransition shot = { n-1, n, CUT };
			keyFrames.push_back(shot);
			logFile << "keyFrame #" << n << endl;
		}
	}

	logFile << " -> EBA (ecr):";
	logFile << " T = " << T << endl;
	logFile << "	Nr of all frames / nr of key frames (" << nrFrames << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}


