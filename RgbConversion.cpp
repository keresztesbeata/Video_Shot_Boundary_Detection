#include "stdafx.h"
#include "RgbConversion.h"

void extractRGBChannelsFromColourImage(Mat_<Vec3b> img, Mat_<uchar>& r_img, Mat_<uchar>& g_img, Mat_<uchar>& b_img) {
	int height = img.rows;
	int width = img.cols;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			r_img(i, j) = img(i, j)[2];
			g_img(i, j) = img(i, j)[1];
			b_img(i, j) = img(i, j)[0];
		}
	}
}

void testCopyRGB() {
	char fname[MAX_PATH];
	if (openFileDlg(fname) < 0) {
		return;
	}
	Mat img = imread(fname, IMREAD_UNCHANGED);

	int height = img.rows;
	int width = img.cols;

	Mat_<uchar> r_img(height, width);
	Mat_<uchar> g_img(height, width);
	Mat_<uchar> b_img(height, width);

	extractRGBChannelsFromColourImage(img, r_img, g_img, b_img);

	imshow("red", r_img);
	imshow("green", g_img);
	imshow("blue", b_img);

	waitKey(0);
}

unsigned char RGBtoGrayScale(Vec3b pixel) {
	return (pixel[0] + pixel[1] + pixel[2]) / 3;
}

Mat_<uchar> convertRgbToGrayscale(Mat_<Vec3b> src) {
	int height = src.rows;
	int width = src.cols;
	Mat_<uchar> dst(height, width);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			dst(i, j) = RGBtoGrayScale(src(i, j));
		}
	}

	return dst;
}

unsigned char grayScaleToBlackAndWHite(unsigned char grayScaleValue, int threshold) {
	return (grayScaleValue < threshold) ? 0:255;
}

Mat_<uchar> convertGrayScaleToBlackAndWhite(Mat_<uchar> src, int threshold) {
	int height = src.rows;
	int width = src.cols;
	Mat_<uchar> dst(height, width);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			dst(i, j) = grayScaleToBlackAndWHite(src(i, j), threshold);
		}
	}

	return dst;
}

Vec3f RGBtoHSV(Vec3b rgb) {
	float r = rgb[2];
	float g = rgb[1];
	float b = rgb[0];

	// normalized rgb components
	r /= 255.0;
	g /= 255.0;
	b /= 255.0;

	float M = max(r, max(g, b));
	float m = min(r, min(g, b));
	float C = M - m;

	float H, S, V;

	// Value
	V = M;

	// saturation
	if (V != 0) {
		S = C / V;
	}
	else {
		// black
		S = 0;
	}

	// Hue
	if (C != 0) {
		if (M == r) H = 60 * (g - b) / C;
		if (M == g) H = 120 + 60 * (b - r) / C;
		if (M == b) H = 240 + 60 * (r - g) / C;
	}
	else {
		// grayscale
		H = 0;
	}

	if (H < 0) {
		H = H + 360;
	}

	Vec3f hsv = { H,S,V };
	
	return hsv;
}

void convertRgbToHsv(Mat_<Vec3b> img, Mat_<uchar>& h_img, Mat_<uchar>& s_img, Mat_<uchar>& v_img) {
	for (int i = 0; i < img.rows; i++) {
		for (int j = 0; j < img.cols; j++) {
			Vec3f hsv = RGBtoHSV(img(i, j));

			// scale HSV components in order to display them as 8-bit grayscale images
			h_img(i, j) = hsv[0] * 255 / 360;
			s_img(i, j) = hsv[1] * 255;
			v_img(i, j) = hsv[2] * 255;
		}
	}
}
