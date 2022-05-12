#include "stdafx.h"
#include "Histogram.h"

int * computeHistogram(Mat_<uchar> img) {
	int* hist = (int*)malloc(sizeof(int) * 256);
	// initialize the frequency array (count gray level intensities)
	for (int i = 0; i < 256; i++) {
		hist[i] = 0;
	}
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			// number of pixels with a given gray level
			hist[img(i, j)]++;
		}
	}

	return hist;
}
 
int * computeCumulativeHistogram(Mat_<uchar> img) {
	int* hist = computeHistogram(img);

	for (int i = 1; i < 256; i++) {
		hist[i] += hist[i - 1];
	}

	return hist;
}

float * computePDF(Mat_<uchar> img) {
	int* hist = computeHistogram(img);
	float* pdf = (float*)malloc(sizeof(float) * 256);

	for (int i = 0; i < 256; i++) {
		pdf[i] = (float)hist[i] / 255.0;
	}

	return pdf;
}

float* computeCumulativePDF(Mat_<uchar> img) {
	float* pdf = computePDF(img);

	for (int i = 1; i < 256; i++) {
		pdf[i] += pdf[i - 1];
	}

	return pdf;
}

int * computeHistogramWithBins(Mat_<uchar> img, int nr_bins) {
	int* hist = (int*)malloc(sizeof(int) * nr_bins);
	// initialize the histogram array
	for (int i = 0; i < nr_bins; i++) {
		hist[i] = 0;
	}
	int range = 256 / nr_bins;
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			// each pixel belongs to a bin based on its gray level intensity
			hist[img(i, j) / range]++;
		}
	}

	return hist;
}

int*** computeColorHistogram(Mat_<Vec3b> img) {
	int*** hist = (int***)malloc(sizeof(int**) * 256);
	for (int i = 0; i < 256; i++) {
		hist[i] = (int**)malloc(sizeof(int*) * 256);
		for (int j = 0; j < 256; j++) {
			hist[i][j] = (int*)malloc(sizeof(int) * 256);
			for (int k = 0; k < 256; k++) {
				hist[i][j][k] = 0;
			}
		}
	}
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			int r = img(i, j)[2];
			int g = img(i, j)[1];
			int b = img(i, j)[0];
			hist[r][g][b]++;
		}
	}

	return hist;
}

void showHistogram(const char* name, int* hist, const int hist_height, const int hist_cols) {
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_height; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];

	double scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(0, 0, 0));
	}
	imshow(name, imgHist);
}