#include "stdafx.h"
#include "StatisticalProperties.h"


int findMinIntensity(int* hist);
int findMaxIntensity(int* hist);

float computeMeanIntensitiyValue(Mat_<uchar> img) {
	// compute histogram for given image
	int * h = computeHistogram(img);

	// number of pixels
	int M = img.rows * img.cols;

	// maximum grey level
	int L = 255;

	float mean = 0.0;
	for (int g = 0; g <= L; g++) {
		mean += (float)g * h[g];
	}

	return mean / (float)M;
}

float computeStandardDeviation(Mat_<uchar> img) {
	// maximum grey level
	int height = img.rows;
	int width = img.cols;

	int M = height * width;

	// compute mean value
	float mean = computeMeanIntensitiyValue(img);

	// compute standard deviation
	float standardDeviation = 0.0;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			standardDeviation += (img(i, j) - mean) * (img(i, j) - mean);
		}
	}

	standardDeviation /= (float)M;

	return sqrt(standardDeviation);
}

int findMinIntensity(int* hist) {
	int g = 0;
	while (hist[g] == 0 && g < 255) { g++; }
	return g;
}


int findMaxIntensity(int* hist) {
	int g = 255;
	while (hist[g] == 0 && g > 0) { g--; }
	return g;
}


Mat_<uchar> basicTresholding(Mat_<uchar> src) {
	// compute histogram
	int * h = computeHistogram(src);

	// find min intensity using histogram
	int minIntensity = findMinIntensity(h);
	
	// find max intensity using histogram
	int maxIntensity = findMaxIntensity(h);

	// compute the initial threshold
	float T = (minIntensity + maxIntensity) / 2.0;
	float prevT = 0;
	float error = 0.1;

	while (abs(T - prevT) < error) {

		// divide the image into 2 groups based on the threshold

		float mean1 = 0.0;
		int N1 = 0;
		for (int g = minIntensity; g <= T; g++) {
			N1 += h[g];
			mean1 += (float)g * h[g];
		}
		mean1 /= (float)N1;

		float mean2 = 0.0;
		int N2 = 0;
		for (int g = T; g <= maxIntensity; g++) {
			N2 += h[g];
			mean2 += (float)g * h[g];
		}
		mean2 /= (float)N2;

		// update the threshold values
		prevT = T;
		T = (mean1 + mean2) / 2.0;
	}

	// threshold the image using T
	return convertGrayScaleToBlackAndWhite(src, T);
}

int getMinIntensityLevel(Mat_<uchar> src) {
	int* hist = computeHistogram(src);

	int i = 0;
	while (i < 256 && hist[i] == 0) {
		i++;
	}
	return i;
}

int getMaxIntensityLevel(Mat_<uchar> src) {
	int* hist = computeHistogram(src);

	int i = 255;
	while (i >= 0 && hist[i] == 0) {
		i--;
	}
	return i;
}