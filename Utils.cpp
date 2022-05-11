#include "stdafx.h"
#include "Utils.h"

bool isInside(Mat img, int i, int j) {
	return i >= 0 && i < img.rows&& j >= 0 && j < img.cols;
}

float getMeanOfRegion(vector<float> difference, int start, int end) {
	float mean = 0;
	for (int i = start; i < end; i++) {
		mean += difference[i];
	}

	return mean / (float)(end - start);
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
