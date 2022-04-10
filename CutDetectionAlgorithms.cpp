#include "stdafx.h"
#include "CutDetectionAlgorithms.h"

vector<Mat> pixelBasedApproach(const char * fileName, float threshold) {
	vector<Mat> keyFrames;
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
			getPBADissimilarityForGrayScale(previousFrame, currentFrame) 
			: 
			getPBADissimilarityForColour(previousFrame, currentFrame)
			;

		if (d > threshold) {
			keyFrames.push_back(previousFrame);
		}

		previousFrame = currentFrame.clone();
		
		nrFrames++;
	}

	cout<<"Nr of all frames / nr of key frames ("<< nrFrames<<","<< keyFrames.size()<<")"<<endl;

FINISH:
	return keyFrames;
}


float getPBADissimilarityForGrayScale(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame) {
	int rows = previousFrame.rows;
	int cols = previousFrame.cols;

	float sum = 0;
	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < cols; y++) {
			sum += abs(previousFrame(x, y) - currentFrame(x, y));
		}
	}

	return sum / (float)(rows * cols);
}

float getPBADissimilarityForColour(Mat_<Vec3b> previousFrame, Mat_<Vec3b> currentFrame) {
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

	float d_r = getPBADissimilarityForGrayScale(r_prev, r_curr);
	float d_g = getPBADissimilarityForGrayScale(g_prev, g_curr);
	float d_b = getPBADissimilarityForGrayScale(b_prev, b_curr);

	return d_r + d_g + d_b;
}