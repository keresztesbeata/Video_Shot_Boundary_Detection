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

int * computeHistogramWithBins(Mat_<uchar> img, int nr_bins) {
	int* hist = (int*)malloc(sizeof(int) * 256);
	// initialize the histogram array
	for (int i = 0; i < 256; i++) {
		hist[i] = 0;
	}
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			// each pixel belongs to a bin based on its gray level intensity
			int range = 256 / nr_bins;
			hist[img(i, j) / range]++;
		}
	}

	return hist;
}

std::vector<int> getHistogramMaxima(float* pdf) {
	std::vector<int> histMax;
	const int WH = 5;
	const float TH = 0.0003;

	float windowWidth = 2 * WH + 1;
	float windowHeight = TH;

	histMax.push_back(0);

	for (int k = WH; k <= 255 - WH; k++) {
		float v = 0;
		float maxH = -1;
		for (int h = k - WH; h <= k + WH; h++) {
			v += pdf[h];
			if (pdf[h] > maxH) {
				maxH = pdf[h];
			}
		}
		v /= float(2 * WH + 1);

		if (pdf[k] > v + TH && pdf[k] >= maxH) {
			histMax.push_back(k);
		}
	}
	histMax.push_back(255);

	return histMax;
}

int getClosestLocalMaximum(int k, std::vector<int> histMax) {
	int difference = 255;
	int nearestMax = 0;

	for (int h : histMax) {
		if (abs(k - h) < difference) {
			nearestMax = h;
			difference = abs(k - h);
		}
	}
	return nearestMax;
}

/*
* Determine multiple thresholds for reducing the gray levels:
* 1. determine the local maxima of the histogram
* 2. assign each gray level to the closest max
*/
void multilevelThresholding(Mat_<uchar> img, Mat_<uchar>& result_img) {
	float * pdf = computePDF(img);

	std::vector<int> histMax = getHistogramMaxima(pdf);

	printf("Maximums = \n");
	for (int m : histMax) {
		printf("%d ", m);
	}
	printf("\n");

	int height = img.rows;
	int width = img.cols;

	int lut[256];
	for (int i = 0; i < 256; i++) {
		lut[i] = getClosestLocalMaximum(i, histMax);
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			result_img(i, j) = lut[img(i, j)];
		}
	}
}

/*
* Add an error offset to the yet unvisited neighburs of the pixel.
*/
void floydSteinbergDithering(Mat_<uchar>& img) {
	int height = img.rows;
	int width = img.cols;

	float* pdf = computePDF(img);

	std::vector<int> histMax = getHistogramMaxima(pdf);

	int lut[256];
	for (int i = 0; i < 256; i++) {
		lut[i] = getClosestLocalMaximum(i, histMax);
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int oldpixel = img(i, j);
			int newpixel = lut[oldpixel];
			img(i, j) = newpixel;
			int error = oldpixel - newpixel;
			int newValue;
			if (isInside(img, i, j + 1)) {
				newValue = img(i, j + 1) + 7 * error / 16;
				img(i, j + 1) = max(0, min(255, newValue));
			}
			if (isInside(img, i + 1, j - 1)) {
				newValue = img(i + 1, j - 1) + 3 * error / 16;
				img(i + 1, j - 1) = max(0, min(255, newValue));
			}
			if (isInside(img, i + 1, j)) {
				newValue = img(i + 1, j) + 5 * error / 16;
				img(i + 1, j) = max(0, min(255, newValue));
			}
			if (isInside(img, i + 1, j + 1)) {
				newValue = img(i + 1, j + 1) + error / 16;
				img(i + 1, j + 1) = max(0, min(255, newValue));
			}
		}
	}
}

Mat_<uchar> histogramEqualization(Mat_<uchar> src) {
	float * cpdf = computeCumulativePDF(src);
	float s[256];

	for (int k = 0; k < 256; k++) {
		// k is the un-normalized intensity level
		// s[k] is the normalized intensity level of the output image
		s[k] = 255 * cpdf[k];
	}
	
	Mat_<uchar> dst(src.rows, src.cols);

	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			dst(i, j) = s[src(i, j)];
		}
	}

	return dst;
}