#include "stdafx.h"
#include "GradualTransitionDetector.h"
#include "StatisticalProperties.h"
#include "EBAalgorithms.h"

using namespace std;

int getHistDifference(Mat_<uchar> previousFrame, Mat_<uchar> currentFrame, int nrBins) {
	int* histPrev = computeHistogramWithBins(previousFrame, nrBins);
	int* histCurr = computeHistogramWithBins(currentFrame, nrBins);

	int difference = 0;

	for (int g = 0; g < nrBins; g++) {
		difference += histCurr[g] - histPrev[g];
	}

	return difference;
}

vector<int> findMonochromeFrames(vector<Mat> frames, float T) {
	vector<int> monochrome;
	int n = frames.size();
	for (int i = 0; i < n; i++) {
		Mat_<uchar> currFrame = convertRgbToGrayscale(frames[i]);
		float stdDev = computeStandardDeviation(currFrame);
		if (stdDev < T) {
			monochrome.push_back(i);
		}
	}
	return monochrome;
}



vector<FrameTransition> detectFadeTransitions_v1(vector<Mat> frames, float maxStdDev, float maxChange, int minLength, ofstream& logFile) {
	
	logFile << " -> Detect fade in/out (Based on Mean and Standard Deviation):"<<endl;
	logFile << " max std dev = " << maxStdDev << endl;
	logFile << " max intensity change = " << maxChange << endl;
	logFile << " min transition length = " << minLength << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

	int nrFrames = frames.size();
	Mat previousFrame, currentFrame;
	vector<FrameTransition> fades;

	vector<int> monochromes = findMonochromeFrames(frames, maxStdDev);
	int nrPotentialFades = monochromes.size();

	for (int i = 0; i < nrPotentialFades; i++) {
		logFile << "monochrome #" << monochromes[i] << endl;
		// detect fade-in (the image gradually appears) => monotonically increasing luminance

		int start = monochromes[i];
		int end = start;
		Vec3f refMean = computeMeanIntensitiyValueColor(frames[start]);
		float totalRefMean = refMean[0] + refMean[1] + refMean[2];
		for (int u = start + 1; u < nrFrames; u++) {
			Vec3f currMean = computeMeanIntensitiyValueColor(frames[u]);
			float totalCurrMean = currMean[0] + currMean[1] + currMean[2];
			float diff_r = currMean[0] - refMean[0];
			float diff_g = currMean[1] - refMean[1];
			float diff_b = currMean[2] - refMean[2];
			//logFile << "fade out"<< diff_r << " " << diff_g << " " << diff_b << endl;
			/*
			if ((diff_r >= 0 && diff_b >= 0 && diff_g >= 0) || (abs(diff_r) > maxChange && abs(diff_g) > maxChange && abs(diff_b) > maxChange)) {
				// if the luminance doesn't change monotonically anymore or the change in luminance is not gradual, then stop and mark the end of the fade transition
				end = u;
				break;
			}
			*/

			// std dev is increasing, mean is descreasing
			if (totalCurrMean > totalRefMean && (totalCurrMean - totalRefMean > maxChange)) {
				// if the luminance doesn't change monotonically anymore or the change in luminance is not gradual, then stop and mark the end of the fade transition
				end = u;
				break;
			}

		}

		if ((end - start) >= minLength) {
			FrameTransition gt = { start, end, FADE_IN };
			fades.push_back(gt);
			logFile <<"fade in frames[" << start << ", " << end << "]" << endl;
		}

		// detect fade-out (the new image gradually disappears) => monotonically decreasing luminance

		end = monochromes[i];
		for (int u = end - 1; u >= 0; u--) {
			Vec3f currMean = computeMeanIntensitiyValue(frames[u]);
			float totalCurrMean = currMean[0] + currMean[1] + currMean[2];
			float diff_r = currMean[0] - refMean[0];
			float diff_g = currMean[1] - refMean[1];
			float diff_b = currMean[2] - refMean[2];
			// std dev is decreasing, mean is increasing
			//logFile << "fade in" << diff_r << " " << diff_g << " " << diff_b << endl;
			if (totalCurrMean > totalRefMean && (abs(diff_r) > maxChange || abs(diff_g) > maxChange || abs(diff_b) > maxChange)) {
				// if the luminance doesn't change monotonically anymore or the change in luminance is not gradual, then stop and mark the end of the fade transition
				start = u;
				break;
			}
		}

		if ((end-start) >= minLength) {
			FrameTransition gt = { start, end, FADE_OUT };
			fades.push_back(gt);
			logFile << "fade out frames [" << start << ", " << end << "]" << endl;
		}
	}
	logFile << " --------------------------------------------------------------------------------------" << endl;

	return fades;
}



vector<FrameTransition> detectFadeTransitions_v2(vector<Mat> frames, float maxStdDev, float maxChange, int minLength, ofstream& logFile) {

	logFile << " -> Detect fade in/out (Based on Mean and Standard Deviation):" << endl;
	logFile << " max std dev = " << maxStdDev << endl;
	logFile << " max intensity change = " << maxChange << endl;
	logFile << " min transition length = " << minLength << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

	int nrFrames = frames.size();
	Mat previousFrame, currentFrame;
	vector<FrameTransition> fades;

	vector<int> monochromes = findMonochromeFrames(frames, maxStdDev);
	int nrPotentialFades = monochromes.size();

	for (int i = 0; i < nrPotentialFades; i++) {
		logFile << "monochrome #" << monochromes[i] << endl;
		// detect fade-in (the image gradually appears) => monotonically increasing luminance

		int start = monochromes[i];
		int end = start;
		Vec3f refMean = computeMeanIntensitiyValueColor(frames[start]);
		float totalRefMean = refMean[0] + refMean[1] + refMean[2];
		for (int u = start + 1; u < nrFrames; u++) {
			Vec3f currMean = computeMeanIntensitiyValueColor(frames[u]);
			float totalCurrMean = currMean[0] + currMean[1] + currMean[2];
			float diff_r = currMean[0] - refMean[0];
			float diff_g = currMean[1] - refMean[1];
			float diff_b = currMean[2] - refMean[2];
			//logFile << "fade out"<< diff_r << " " << diff_g << " " << diff_b << endl;
			/*
			if ((diff_r >= 0 && diff_b >= 0 && diff_g >= 0) || (abs(diff_r) > maxChange && abs(diff_g) > maxChange && abs(diff_b) > maxChange)) {
				// if the luminance doesn't change monotonically anymore or the change in luminance is not gradual, then stop and mark the end of the fade transition
				end = u;
				break;
			}
			*/

			// std dev is increasing, mean is descreasing
			if (totalCurrMean > totalRefMean && (totalCurrMean - totalRefMean > maxChange)) {
				// if the luminance doesn't change monotonically anymore or the change in luminance is not gradual, then stop and mark the end of the fade transition
				end = u;
				break;
			}

		}

		if ((end - start) >= minLength) {
			FrameTransition gt = { start, end, FADE_IN };
			fades.push_back(gt);
			logFile << "fade in frames[" << start << ", " << end << "]" << endl;
		}

		// detect fade-out (the new image gradually disappears) => monotonically decreasing luminance

		end = monochromes[i];
		for (int u = end - 1; u >= 0; u--) {
			Vec3f currMean = computeMeanIntensitiyValue(frames[u]);
			float totalCurrMean = currMean[0] + currMean[1] + currMean[2];
			float diff_r = currMean[0] - refMean[0];
			float diff_g = currMean[1] - refMean[1];
			float diff_b = currMean[2] - refMean[2];
			// std dev is decreasing, mean is increasing
			//logFile << "fade in" << diff_r << " " << diff_g << " " << diff_b << endl;
			if (totalCurrMean > totalRefMean && (abs(diff_r) > maxChange || abs(diff_g) > maxChange || abs(diff_b) > maxChange)) {
				// if the luminance doesn't change monotonically anymore or the change in luminance is not gradual, then stop and mark the end of the fade transition
				start = u;
				break;
			}
		}

		if ((end - start) >= minLength) {
			FrameTransition gt = { start, end, FADE_OUT };
			fades.push_back(gt);
			logFile << "fade out frames [" << start << ", " << end << "]" << endl;
		}
	}
	logFile << " --------------------------------------------------------------------------------------" << endl;

	return fades;
}


// check if it is fading to/from black
bool isFading(int minPrev, int maxPrev, int minCurr, int maxCurr) {
	return maxPrev <= maxCurr && minPrev <= minCurr;
}

vector<FrameTransition> detectFadeTransitions_v3(vector<Mat> frames, float maxStdDev, int minLength, ofstream& logFile) {

	logFile << " -> Detect fade in/out (Based on histogram difference):"<<endl;
	logFile << " min transition length = " << minLength << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

	Mat previousFrame, currentFrame;
	vector<FrameTransition> fades;
	int nrFrames = frames.size();

	vector<int> gmin;
	vector<int> gmax;
	vector<int> monochromes;

	for (int i = 0; i < nrFrames; i++) {
		Mat_<uchar> currFrame = convertRgbToGrayscale(frames[i]);
		float stdDev = computeStandardDeviation(currFrame);
		int max = getMaxIntensityLevel(currFrame);
		gmax.push_back(max);
		int min = getMinIntensityLevel(currFrame);
		gmin.push_back(min);
		if (stdDev < maxStdDev) {
			monochromes.push_back(i);
		}
	}

	int nrPotentialFades = monochromes.size();
	int lastTransitionFrameIdx = 0;

	for (int i = 0; i < nrPotentialFades; i++) {
		int monoChromeIdx = monochromes[i];
		if (monoChromeIdx <= lastTransitionFrameIdx) {
			i++;
			continue;
		}
		
		// analyze frames on the left to detect start of a fade out
		int j = monoChromeIdx;
		while (j > 0 && isFading(gmin[j], gmax[j], gmin[j - 1], gmax[j - 1])) {
			j--;
		}

		if (abs(monoChromeIdx - j) > minLength) {
			FrameTransition ft = { j, monoChromeIdx, FADE_OUT };
			fades.push_back(ft);
			logFile << "fade out frames [" << j << ", " << monoChromeIdx << "]" << endl;
			lastTransitionFrameIdx = monoChromeIdx;
		}
		
		// analyze frames on the right to detect start of a fade in
		j = monoChromeIdx;
		while (j < nrFrames-1 && isFading(gmin[j], gmax[j], gmin[j + 1], gmax[j + 1])) {
			j++;
		}

		if (abs(monoChromeIdx - j) > minLength) {
			FrameTransition ft = { monoChromeIdx, j, FADE_IN };
			fades.push_back(ft);
			logFile << "fade in frames [" << monoChromeIdx << ", " << j << "]" << endl;
			lastTransitionFrameIdx = j;
		}
	}

	logFile << " --------------------------------------------------------------------------------------" << endl;

	return fades;
}
