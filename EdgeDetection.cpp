#include "stdafx.h"
#include "EdgeDetection.h"
#include "stdafx.h"
#include "EdgeDetection.h"
#include "Histogram.h"
#include <queue>

/*
Mat cannyEdgeDetector(Mat src) {
	return cannyEdgeDetection(src, 0.1, 0.4);
}
*/
Mat cannyEdgeDetector(Mat src) {

    if (src.channels() == 3) {
        // colour image should be converted to grayscale
        cvtColor(src, src, COLOR_BGR2GRAY);
    }
    blur(src, src, Size(3, 3));
    int lowThreshold = 0;
    int ratio = 3;
    int kernel_size = 3;
    Mat edges, detected_edges;
    edges = Scalar::all(0);
    Canny(src, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
    src.copyTo(edges, detected_edges);
    
    return edges;
}

bool isEdgePixel(Mat img, int i, int j) {
    return img.at<uchar>(i, j) > 0;
}


Mat_<float> convolution(Mat_<uchar> src, int w, Mat_<float> H) {
	int height = src.rows;
	int width = src.cols;

	// kernel has symmetric dimension = w x w, w = 2k+1
	int k = (w - 1) / 2;

	Mat_<float> dst(height, width);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			float sum = 0;
			for (int u = 0; u < w; u++) {
				for (int v = 0; v < w; v++) {
					if (isInside(src, i + u - k, j + v - k)) {
						sum += H(u, v) * src(i + u - k, j + v - k);
					}
				}
			}
			dst(i, j) = sum;
		}
	}

	return dst;
}


Mat_<uchar> gaussianNoiseFilter(Mat_<uchar> src, int w) {
	int height = src.cols;
	int width = src.rows;
	float phi = (float)w / 6;

	Mat_<float> G(w, w);
	int c = w / 2;
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < w; y++) {
			float coeff = -((x - c) * (x - c) + (y - c) * (y - c)) / (2 * phi * phi);
			G(x, y) = 1.0 / (2 * 3.14 * phi * phi) * exp(coeff);
		}
	}
	Mat_<uchar> dst = convolution(src, w, G);

	//Mat_<uchar> dst = normalizeFilter(conv, w, G, HIGH_PASS);

	return dst;
}


void computeGradient(Mat_<uchar> src, KernelType kernel, Mat_<float>& Fx, Mat_<float>& Fy) {
	switch (kernel) {
	case PREWITT: {
		float prewittKernelX[] = { -1,0,1,-1,0,1,-1,0,1 };
		Mat HX(3, 3, CV_32F, prewittKernelX);
		Fx = convolution(src, 3, HX);
		float prewittKernelY[] = { 1, 1, 1, 0, 0, 0, -1, -1, -1 };
		Mat HY(3, 3, CV_32F, prewittKernelY);
		Fy = convolution(src, 3, HY);
		break;
	}
	case SOBEL: {
		float sobelKernelX[] = { -1,0,1,-2,0,2,-1,0,1 };
		Mat HX(3, 3, CV_32F, sobelKernelX);
		Fx = convolution(src, 3, HX);
		float sobelKernelY[] = { 1, 2, 1, 0, 0, 0, -1, -2, -1 };
		Mat HY(3, 3, CV_32F, sobelKernelY);
		Fy = convolution(src, 3, HY);
		break;
	}
	case ROBERT: {
		float robertKernelX[] = { -1,0,1,-1 };
		Mat HX(2, 2, CV_32F, robertKernelX);
		Fx = convolution(src, 2, HX);
		float robertKernelY[] = { 0,-1,1,0 };
		Mat HY(2, 2, CV_32F, robertKernelY);
		Fy = convolution(src, 2, HY);
		break;
	}
	}
}

void computeGradientMagnitudeAndDirection(Mat_<uchar> src, KernelType kernel, Mat_<float>& mag, Mat_<float>& phi) {
	Mat_<float> Fx, Fy;
	switch (kernel) {
	case PREWITT: {
		computeGradient(src, PREWITT, Fx, Fy);
		break;
	}
	case SOBEL: {
		computeGradient(src, SOBEL, Fx, Fy);
		break;
	}
	case ROBERT: {
		computeGradient(src, ROBERT, Fx, Fy);
		break;
	}
	}

	int height = src.rows;
	int width = src.cols;
	mag = Mat(height, width, CV_32F);
	phi = Mat(height, width, CV_32F);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			mag(i, j) = sqrt(Fx(i, j) * Fx(i, j) + Fy(i, j) * Fy(i, j));
			phi(i, j) = atan2(Fy(i, j), Fx(i, j));
		}
	}
}

int getOrientation(float radian) {
	// radian is in the interval (-PI, PI), so we first transform it to the interval (0,2*PI)

	float degree = ((float)radian + CV_PI) * 180.0 / (float)CV_PI;

	if (degree > 180) {
		degree -= 180;
	}

	float offset = 22.5;
	if ((degree >= 90 - offset && degree <= 90 + offset) || (degree >= 270 - offset && degree <= -270 + offset)) {
		return 0;
	}
	if ((degree >= 45 - offset && degree <= 45 + offset) || (degree >= 225 - offset && degree <= 225 + offset)) {
		return 1;
	}
	if ((degree >= 180 - offset && degree <= 180 + offset) || (degree >= 360 - offset && degree <= 0 + offset)) {
		return 2;
	}
	if ((degree >= 135 - offset && degree <= 135 + offset) || (degree >= 315 - offset && degree <= 315 + offset)) {
		return 3;
	}
}

Mat_<uchar> nonMaximaSuppression(Mat_<uchar> mag, Mat_<float> phi) {
	int height = mag.rows;
	int width = mag.cols;

	Mat_<uchar> dst = mag.clone();

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int orientation = getOrientation(phi(i, j));
			int x1 = i, x2 = i, y1 = j, y2 = j;
			switch (orientation) {
			case 0: {
				x1 = i - 1;
				y1 = j;
				x2 = i + 1;
				y2 = j;
				break;
			}
			case 1: {
				x1 = i - 1;
				y1 = j + 1;
				x2 = i + 1;
				y2 = j - 1;
				break;
			}
			case 2: {
				x1 = i;
				y1 = j - 1;
				x2 = i;
				y2 = j + 1;
				break;
			}
			case 3: {
				x1 = i - 1;
				y1 = j - 1;
				x2 = i + 1;
				y2 = j + 1;
				break;
			}
			default: break;
			}
			if ((isInside(mag, x1, y1) && mag(i, j) < mag(x1, y1)) || (isInside(mag, x2, y2) && mag(i, j) < mag(x2, y2))) {
				dst(i, j) = 0;
			}
		}
	}
	return dst;
}


Mat_<uchar> normalize(Mat_<float> img, KernelType kernelType) {
	int height = img.rows;
	int width = img.cols;
	Mat_<uchar> dst(height, width);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int result = 0;
			float factor = 1.0;
			switch (kernelType) {
			case PREWITT: {
				factor = 3 * sqrt(2);
				break;
			}
			case SOBEL: {
				factor = 4 * sqrt(2);
				break;
			}
			case ROBERT: {
				factor = 2 * sqrt(2);
				break;
			}
			}
			dst(i, j) = img(i, j) / factor;
		}
	}

	return dst;
}

float computeAdaptiveThreshold(Mat_<uchar> mag, float p) {
	int hist[256];
	for (int i = 0; i < 256; i++) {
		hist[i] = 0;
	}
	int height = mag.rows;
	int width = mag.cols;

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			hist[mag(i, j)]++;
		}
	}
	// nr of pixels with non-zero values which would not be edge points
	int nrNonEdge = (1 - p) * (height * width - hist[0]);
	int sum = 0;
	for (int i = 1; i < 256; i++) {
		sum += hist[i];
		if (sum >= nrNonEdge) {
			return i;
		}
	}

	return 0;
}

Mat_<uchar> thresholding(Mat_<uchar> mag, float p, float k) {
	float THigh = computeAdaptiveThreshold(mag, p);
	float TLow = k * THigh;

	int height = mag.rows;
	int width = mag.cols;

	Mat_<int> dst(height, width);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (mag(i, j) >= THigh) {
				// STRONG EDGES
				dst(i, j) = STRONG_EDGE;
			}
			else if (mag(i, j) >= TLow) {
				// WEAK EDGES
				dst(i, j) = WEAK_EDGE;
			}
			else {
				// NON EDGES
				dst(i, j) = NON_EDGE;

			}
		}
	}
	return dst;
}

Mat_<uchar> convert(Mat_<uchar> img) {
	int height = img.rows;
	int width = img.cols;

	Mat_<uchar> dst(height, width);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			switch (img(i, j)) {
			case STRONG_EDGE: {
				dst(i, j) = 255;
				break;
			}
			case WEAK_EDGE: {
				dst(i, j) = 128;
				break;
			}
			case NON_EDGE: {
				dst(i, j) = 0;
				break;
			}
			}
		}
	}
	return dst;
}

void edgeExtension(Mat_<uchar>& img) {
	int height = img.rows;
	int width = img.cols;

	// extend STRONG EDGE points with neighbouring WEAK_EDGE points if they are in the same connected component

	Mat_<uchar> visited(height, width);
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			visited(i, j) = 0;
		}
	}

	Point2i firstStrongPoint;
	bool found = false;
	for (int i = 0; i < height && !found; i++) {
		for (int j = 0; j < width && !found; j++) {
			if (img(i, j) == STRONG_EDGE) {
				firstStrongPoint = { i,j };
				found = true;
			}
		}
	}


	std::queue<Point2i> q;
	q.push(firstStrongPoint);

	int dx[] = { -1,-1,-1,0,0,1,1,1 };
	int dy[] = { -1,0,1,-1,1,-1,0,1 };
	int N = 8;

	while (!q.empty()) {
		Point2i strongPoint = q.front();
		q.pop();
		int x = strongPoint.x;
		int y = strongPoint.y;
		for (int i = 0; i < N; i++) {
			if (isInside(img, x + dx[i], y + dy[i])) {
				if (img(x + dx[i], y + dy[i]) == WEAK_EDGE) {
					img(x + dx[i], y + dy[i]) = STRONG_EDGE;
					q.push(Point2i(x + dx[i], y + dy[i]));
				}
				else if (img(x + dx[i], y + dy[i]) == STRONG_EDGE && visited(x + dx[i], y + dy[i]) == 0) {
					visited(x + dx[i], y + dy[i]) = 1;
					q.push(Point2i(x + dx[i], y + dy[i]));
				}

			}
		}
	}

	// eliminate remaining non-edge points
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (img(i, j) == WEAK_EDGE) {
				img(i, j) = NON_EDGE;
			}
		}
	}
}

Mat_<uchar> cannyEdgeDetection(Mat_<uchar> src, float p, float k) {
	// step 1: noise filtering
	Mat_<uchar> filteredImg = gaussianNoiseFilter(src, 3);
	// step 2: compute gradient magnitude and direction
	Mat_<float> mag, phi;
	computeGradientMagnitudeAndDirection(filteredImg, SOBEL, mag, phi);
	// normalize gradient magnitude
	Mat_<uchar> magN = normalize(mag, SOBEL);
	imshow("magnitude", magN);
	// step 3: non-maxima suppression
	Mat_<uchar> suppr = nonMaximaSuppression(magN, phi);
	imshow("non-maxima suppression", suppr);
	// step 4: edge linking with adaptive thresholding
	Mat_<uchar> interm = thresholding(suppr, p, k);
	imshow("after thresholding", convert(interm));
	edgeExtension(interm);
	Mat_<uchar> dst = convert(interm);

	return dst;
}
