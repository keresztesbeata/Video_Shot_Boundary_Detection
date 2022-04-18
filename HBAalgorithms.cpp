#include "stdafx.h"
#include "HBAalgorithms.h"

double getBinToBinDifference(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins) {
	int* histPrev = computeHistogramWithBins(previousFrame, nrBins);
	int* histCurr = computeHistogramWithBins(currentFrame, nrBins);

	float difference = 0;
	double sum = 0;

	for (int g = 0; g < nrBins; g++) {
		difference += abs(histPrev[g] - histCurr[g]);
	}

	int N = previousFrame.rows * previousFrame.cols;

	return difference / (double)(2 * N);
}

double getChiSquareTest(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins) {
	int* histPrev = computeHistogramWithBins(previousFrame, nrBins);
	int* histCurr = computeHistogramWithBins(currentFrame, nrBins);

	double difference = 0;

	for (int g = 0; g < nrBins; g++) {
		difference += (double)(histPrev[g] - histCurr[g]) * (histPrev[g] - histCurr[g]) / (double)(histCurr[g] * histCurr[g]);
	}

	int N = previousFrame.rows * previousFrame.cols;

	return difference/(double)(N*N);
}

double getHistogramIntersection(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins) {
	int* histPrev = computeHistogramWithBins(previousFrame, nrBins);
	int* histCurr = computeHistogramWithBins(currentFrame, nrBins);

	double sum = 255;

	for (int g = 0; g < nrBins; g++) {
		sum += (histPrev[g] < histCurr[g]) ? histPrev[g] : histCurr[g];
	}

	int N = previousFrame.rows * previousFrame.cols;

	return 1 - sum/(double)N;
}

double getHDMetricForColourFrames(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame, int nrBins, HDmetric metric) {
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

	double d_r = 0, d_g = 0, d_b = 0;
	int b = nrBins;
		switch (metric) {
		case BIN_TO_BIN_DIFFERENCE: {
			d_r += getBinToBinDifference(r_prev, r_curr, b);
			d_g += getBinToBinDifference(g_prev, g_curr, b);
			d_b += getBinToBinDifference(b_prev, b_curr, b);
			break;
		}
		case CHI_SQUARE_TEST: {
			d_r += getChiSquareTest(r_prev, r_curr, b);
			d_g += getChiSquareTest(g_prev, g_curr, b);
			d_b += getChiSquareTest(b_prev, b_curr, b);
			break;
		}
		case HIST_INTERSECTION: {
			d_r += getHistogramIntersection(r_prev, r_curr, b);
			d_g += getHistogramIntersection(g_prev, g_curr, b);
			d_b += getHistogramIntersection(b_prev, b_curr, b);
			break;
		}
		default: break;
		}

	return d_r + d_g + d_b;
}

vector<pair<int, Mat>> HBA(const char* fileName, float T, ofstream& logFile, HDmetric metric) {
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
	const int nrBinsGrayscale = 256;
	const int nrBinsColour = 64;

	while (videoCapture.read(currentFrame))
	{

		double d = 0;
		switch (metric) {
		case BIN_TO_BIN_DIFFERENCE: {
			d = (nrChannels == 1) ? getBinToBinDifference(previousFrame, currentFrame, nrBinsGrayscale) : getHDMetricForColourFrames(previousFrame, currentFrame, nrBinsColour, BIN_TO_BIN_DIFFERENCE);
			break;
		}
		case CHI_SQUARE_TEST: {
			d = (nrChannels == 1) ? getChiSquareTest(previousFrame, currentFrame, nrBinsGrayscale) : getHDMetricForColourFrames(previousFrame, currentFrame, nrBinsColour, CHI_SQUARE_TEST);
			break;
		}
		case HIST_INTERSECTION: {
			d = (nrChannels == 1) ? getHistogramIntersection(previousFrame, currentFrame, nrBinsGrayscale) : getHDMetricForColourFrames(previousFrame, currentFrame, nrBinsColour, HIST_INTERSECTION);
			break;
		}
		default: break;
		}

		if (d > T) {
			keyFrames.push_back(make_pair(nrFrames, previousFrame));
			logFile << "Key frame #" << nrFrames << endl;
		}

		previousFrame = currentFrame.clone();

		nrFrames++;
	}

	switch (metric) {
	case BIN_TO_BIN_DIFFERENCE: {
		logFile << " -> HBA_v1 (bin-to-bin-difference):";
		break;
	}
	case CHI_SQUARE_TEST: {
		logFile << " -> HBA_v2 (chi-square test):";
		break;
	}
	case HIST_INTERSECTION: {
		logFile << " -> HBA_v3 (histogram intersection):";
		break;
	}
	default: break;
	}
	logFile << " T = " << T << endl;
	logFile << "	Nr of all frames / nr of key frames (" << nrFrames << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}