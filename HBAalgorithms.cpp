#include "stdafx.h"
#include "HBAalgorithms.h"
#include "EBAalgorithms.h"
#include "VideoProcessing.h"

double GDFGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame);
double GDFColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame);

Mat_<uchar> computeRepresentativeFrameGrayScale(Mat_<uchar>* frames, int start, int end);
Mat_<Vec3b> computeRepresentativeFrameColour(Mat_<Vec3b>* frames, int start, int end);

vector<FrameTransition> HBA(string fileName, float T, ofstream& logFile, HDmetric metric) {
	vector<FrameTransition> keyFrames;
	Mat previousFrame, currentFrame;
	int frameNr = 0;

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

	int i = 0;
	while (videoCapture.read(currentFrame))
	{

		double d = 0;
		switch (metric) {
		case BIN_TO_BIN_DIFFERENCE: {
			d = (nrChannels == 1) ? getBinToBinDifference(previousFrame, currentFrame, nrBinsGrayscale) : getHDMetricForColourFrames(previousFrame, currentFrame, nrBinsColour, BIN_TO_BIN_DIFFERENCE);
			break;
		}
		case HIST_INTERSECTION: {
			d = (nrChannels == 1) ? getHistogramIntersection(previousFrame, currentFrame, nrBinsGrayscale) : getHDMetricForColourFrames(previousFrame, currentFrame, nrBinsColour, HIST_INTERSECTION);
			break;
		}
		default: break;
		}

		if (d > T) {
			FrameTransition shot = { frameNr-1, frameNr, CUT };
			keyFrames.push_back(shot);
			logFile << "Key frame #" << frameNr << endl;
		}

		previousFrame = currentFrame.clone();

		frameNr++;
	}

	switch (metric) {
	case BIN_TO_BIN_DIFFERENCE: {
		logFile << " -> HBA_v1 (bin-to-bin-difference):";
		break;
	}
	case HIST_INTERSECTION: {
		logFile << " -> HBA_v2 (histogram intersection):";
		break;
	}
	default: break;
	}
	logFile << " T = " << T << endl;
	logFile << "	Nr of all frames / nr of key frames (" << frameNr << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

FINISH:
	return keyFrames;
}

double getBinToBinDifference(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins) {
	int* histPrev = computeHistogramWithBins(previousFrame, nrBins);
	int* histCurr = computeHistogramWithBins(currentFrame, nrBins);

	double difference = 0;

	for (int g = 0; g < nrBins; g++) {
		difference += abs(histPrev[g] - histCurr[g]);
	}

	int N = previousFrame.rows * previousFrame.cols;

	return difference / (double)(2 * N);
}

double getHistogramIntersection(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins) {
	int* histPrev = computeHistogramWithBins(previousFrame, nrBins);
	int* histCurr = computeHistogramWithBins(currentFrame, nrBins);

	double sum = 0;

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

vector<FrameTransition> HBA_quickShotSearch(string fileName, float T, ofstream& logFile) {
	vector<FrameTransition> keyFrames;
	
	vector<Mat> frames = readAllFrames(fileName);

	int minPartLen = 10;
	int N = frames.size();

	Mat leftRep, rightRep;
	leftRep = computeRepresentativeFrame(frames.data(), 0, minPartLen - 1);
	rightRep = computeRepresentativeFrame(frames.data(), N - minPartLen, N - 1);
	quickShotSearch(frames, leftRep, minPartLen/2, rightRep, N - minPartLen / 2, minPartLen, keyFrames, T, logFile);
	
	logFile << " -> HBA_v4 (quick-sort search):";
	logFile << " T = " << T << endl;
	logFile << "	Nr of all frames / nr of key frames (" << N << "," << keyFrames.size() << ")" << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

	return keyFrames;
}

void quickShotSearch(vector<Mat> frames, Mat leftRep, int leftIdx, Mat rightRep, int rightIdx, int minPartLen, vector<FrameTransition>& shots, double farDissimilarityThreshold, ofstream& logFile) {
	int partLen = rightIdx - leftIdx;
	int midIdx = leftIdx + partLen / 2;
	int shotLoc = 0;
	Mat leftMidRep, rightMidRep;
	shotDetector(frames.data(), midIdx, minPartLen, shotLoc, leftMidRep, rightMidRep);
	
	if (shotLoc > 0) {
		FrameTransition shot = { shotLoc-1, shotLoc, CUT };
		shots.push_back(shot);
		logFile << "Key frame #" << shotLoc << endl;
	}
	
	if (partLen > minPartLen) {
		if (GDF(leftRep, leftMidRep) > farDissimilarityThreshold) {
			quickShotSearch(frames, leftRep, leftIdx, rightMidRep, midIdx, minPartLen, shots, farDissimilarityThreshold, logFile);
		}
		if(GDF(rightMidRep, rightRep) > farDissimilarityThreshold) {
			quickShotSearch(frames, rightMidRep, midIdx, rightRep, rightIdx, minPartLen, shots, farDissimilarityThreshold, logFile);
		}
	}
}

double GDF(Mat previousFrame, Mat currentFrame) {
	int nrChannels = previousFrame.channels();
	if (nrChannels == 1) {
		return GDFGrayScale(previousFrame, currentFrame);
	}
	return GDFColour(previousFrame, currentFrame);
}

double GDFGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame) {
	int nrBins = 256;
	int* histPrev = computeHistogramWithBins(previousFrame, nrBins);
	int* histCurr = computeHistogramWithBins(currentFrame, nrBins);

	double difference = 0;

	for (int g = 0; g < nrBins; g++) {
		double d = abs(histPrev[g] - histCurr[g]);
		difference += d * d;
	}
	return sqrt(difference);
}

double GDFColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame) {
	int height = previousFrame.rows;
	int width = previousFrame.cols;

	int*** histPrev = computeColorHistogram(previousFrame);
	int*** histCurr = computeColorHistogram(currentFrame);

	double difference = 0;
	int nrBins = 256;

	for (int r = 0; r < nrBins; r++) {
		for (int g = 0; g < nrBins; g++) {
			for (int b = 0; b < nrBins; b++) {
				double d = abs(histPrev[r][g][b] - histCurr[r][g][b]);
				difference += d*d;
			}
		}
	}

	return sqrt(difference);
}


int LDF(Mat* frames, int start, int end) {
	double maxDiff = 0;
	int idx = 0;
	for (int i = start; i < end; i++) {
		double d = getHDMetricForColourFrames(frames[i], frames[i + 1], 256, HIST_INTERSECTION);
		if (d > maxDiff) {
			maxDiff = d;
			idx = i + 1;
		}
	}
	return idx;
}

void shotDetector(Mat* frames, int midIdx, int minPartLen, int& shotLoc, Mat& leftMidRep, Mat& rightMidRep) {
	shotLoc = LDF(frames, midIdx - minPartLen/2, midIdx + minPartLen/2);
	if (shotLoc > 0) {
		leftMidRep = computeRepresentativeFrame(frames, midIdx - minPartLen / 2, shotLoc - 1);
		rightMidRep = computeRepresentativeFrame(frames, shotLoc, midIdx + minPartLen / 2);
	}
	else {
		leftMidRep = computeRepresentativeFrame(frames, midIdx - minPartLen / 2, midIdx + minPartLen / 2);
		rightMidRep = leftMidRep;
	}
}

Mat computeRepresentativeFrame(Mat* frames, int start, int end) {
	int nrChannels = frames[start].channels();
	if (nrChannels == 1) {
		return computeRepresentativeFrameGrayScale((Mat_<uchar> *)frames, start, end);
	}
	return computeRepresentativeFrameColour((Mat_<Vec3b> *)frames, start, end);
}

Mat_<Vec3b> computeRepresentativeFrameColour(Mat_<Vec3b> * frames, int start, int end) {
	int height = frames[start].rows;
	int width = frames[start].cols;
	Mat_<Vec3b> rep(height, width);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			vector<Vec3b> v;
			for (int k = start; k <= end; k++) {
				v.push_back(frames[k][i][j]);
			}
			sort(v.begin(), v.end(), [](const Vec3b& lhs, const Vec3b& rhs)
				{
					return (lhs[2] < rhs[2]) || (lhs[2] == rhs[2] && lhs[1] < rhs[1]) || (lhs[2] == rhs[2] && lhs[1] == rhs[1] && lhs[0] < rhs[0]);
				});
			int n = v.size();
			// add the median pixel to the representative frame
			rep[i][j] = v[n / 2];
		}
	}

	return rep;
}

Mat_<uchar> computeRepresentativeFrameGrayScale(Mat_<uchar>* frames, int start, int end) {
	int height = frames[start].rows;
	int width = frames[start].cols;
	Mat_<uchar> rep(height, width);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			vector<uchar> v;
			for (int k = start; k <= end; k++) {
				v.push_back(frames[k][i][j]);
			}
			sort(v.begin(), v.end());
			int n = v.size();
			// add the median pixel to the representative frame
			rep[i][j] = v[n / 2];
		}
	}

	return rep;
}