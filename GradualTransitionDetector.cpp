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


bool isFading(int minPrev, int maxPrev, int minCurr, int maxCurr) {
	// check if it is fading to/from black (or a darker monochrome colour)
	return maxPrev <= maxCurr && minPrev <= minCurr;
}

vector<FrameTransition> detectGradualTransitions_v1(vector<Mat> frames, float maxStdDev, int minLength, ofstream& logFile) {

	logFile << " -> Detect gradual transitions (Based on histogram difference):"<<endl;
	logFile << " max standard deviation = " << maxStdDev << endl;
	logFile << " min transition length = " << minLength << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

	Mat previousFrame, currentFrame;
	vector<FrameTransition> fades;
	int nrFrames = frames.size();

	vector<int> gmin;
	vector<int> gmax;
	vector<int> AHD; // accumulative histogram difference
	vector<int> monochromes;

	AHD.push_back(0);
	for (int i = 0; i < nrFrames; i++) {
		Mat_<uchar> currFrame = convertRgbToGrayscale(frames[i]);
		float stdDev = computeStandardDeviation(currFrame);
		int max = getMaxIntensityLevel(currFrame);
		gmax.push_back(max);
		int min = getMinIntensityLevel(currFrame);
		gmin.push_back(min);
		if (i > 0) {
			AHD.push_back(getHistDifference(frames[i - 1], frames[i], 64));
		}
		if (stdDev < maxStdDev) {
			monochromes.push_back(i);
		}
	}

	int nrPotentialFades = monochromes.size();
	int prevTransitionStart = -1;
	int prevTransitionLength = 0;

	for (int i = 0; i < nrPotentialFades; i++) {
		int monoChromeIdx = monochromes[i];
		if (monochromes[i] >= prevTransitionStart && monochromes[i] <= prevTransitionStart + prevTransitionLength) {
			i++;
			continue;
		}
		// analyze frames on the left to detect start of a fade out
		int j = monoChromeIdx;
		while (j > 0 && isFading(gmin[j], gmax[j], gmin[j - 1], gmax[j - 1]) && AHD[j] <= 0) {
			j--;
		}

		if (abs(monoChromeIdx - j) > minLength) {
			if (j > prevTransitionStart) {
				FrameTransition ft = { j, monoChromeIdx, FADE_OUT };
				fades.push_back(ft);
				logFile << "fade out frames [" << j << ", " << monoChromeIdx << "]" << endl;
				prevTransitionStart = j;
				prevTransitionLength = monoChromeIdx - j + 1;
			}
		}
		
		// analyze frames on the right to detect start of a fade in
		j = monoChromeIdx;
		while (j < nrFrames-1 && isFading(gmin[j], gmax[j], gmin[j + 1], gmax[j + 1]) && AHD[j] >= 0) {
			j++;
		}

		if (abs(j - monoChromeIdx) > minLength) {
			if (monoChromeIdx > prevTransitionStart + prevTransitionLength) {
				// do not overlap with previous fade-in transition
				FrameTransition ft = { monoChromeIdx, j, FADE_IN };
				fades.push_back(ft);
				logFile << "fade in frames [" << monoChromeIdx << ", " << j << "]" << endl;
				prevTransitionStart = monoChromeIdx;
				prevTransitionLength = j - monoChromeIdx + 1;
			}
		}
	}

	logFile << " --------------------------------------------------------------------------------------" << endl;

	return fades;
}

vector<FrameTransition> detectGradualTransitions_v2(vector<Mat> frames, float maxStdDev, int minLength, ofstream& logFile) {

	logFile << " -> Detect gradual transitions (Based on Mean and Standard Deviation):" << endl;
	logFile << " max std dev = " << maxStdDev << endl;
	logFile << " min transition length = " << minLength << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

	int nrFrames = frames.size();
	Mat previousFrame, currentFrame;
	vector<FrameTransition> fades;

	vector<int> monochromes = findMonochromeFrames(frames, maxStdDev);
	int nrPotentialFades = monochromes.size();
	int prevTransitionStart = -1;
	int prevTransitionLength = 0;

	for (int i = 0; i < nrPotentialFades; i++) {

		int interm = monochromes[i];
		int start, end;
		Mat_<uchar> monoChromeFrame = convertRgbToGrayscale(frames[interm]);
		float refStdDev = computeStandardDeviation(monoChromeFrame);
		float currStdDev, prevStdDev;

		// the standard deviation (= "contrast") gradually decreases in the first half of a dissolve transition and in case of a fade-out transition
		start = interm - 1;
		currStdDev = refStdDev;
		prevStdDev = currStdDev;
		while (start >= 0 && prevStdDev - currStdDev >= 0) {
			Mat_<uchar> currFrame = convertRgbToGrayscale(frames[start]);
			currStdDev = computeStandardDeviation(currFrame);
			start--;
			prevStdDev = currStdDev;
		}

		// the standard deviation (= "contrast") gradually increases in the second half of a dissolve transition and in case of a fade-in transition
		end = interm + 1;
		currStdDev = refStdDev;
		prevStdDev = currStdDev;
		while (end < nrFrames && currStdDev - prevStdDev <= 0) {
			Mat_<uchar> currFrame = convertRgbToGrayscale(frames[end]);
			currStdDev = computeStandardDeviation(currFrame);
			end++;
			prevStdDev = currStdDev;
			logFile << "std  dev : "<<prevStdDev << endl;
		}

		if (abs(end - interm) >= minLength / 2 && abs(interm - start) >= minLength / 2) {
			// dissolve
			start = start + 1;
			end = end - 1;
			FrameTransition ft = { start , end, DISSOLVE };
			fades.push_back(ft);
			logFile << "dissolve frames[" << start << ", " << end << "]" << endl;
			prevTransitionStart = start;
			prevTransitionLength = end - start + 1;
		}
		else {
			if (abs(interm - start) >= minLength) {
				// fade-out to black
				if (start > prevTransitionStart) {
					// do not overlap with previous fade-out transition
					end = interm;
					start = start + 1;
					FrameTransition ft = { start , end,  FADE_OUT };
					fades.push_back(ft);
					logFile << "fade out frames[" << start << ", " << end << "]" << endl;
					prevTransitionStart = start;
					prevTransitionLength = end - start + 1;
				}
			}
			else if (abs(end - interm) >= minLength) {
				// fade-in from black
				start = interm;
				end = end - 1;
				if (start > prevTransitionStart + prevTransitionLength) {
					// do not overlap with previous fade-in transition
					FrameTransition ft = { start , end,  FADE_IN };
					fades.push_back(ft);
					logFile << "fade in frames[" << start << ", " << end << "]" << endl;
					prevTransitionStart = start;
					prevTransitionLength = end - start + 1;
				}
			}
		}
	}
	logFile << " --------------------------------------------------------------------------------------" << endl;

	return fades;
}

vector<FrameTransition> detectGradualTransitions_v3(vector<Mat> frames, float maxStdDev, int minLength,  ofstream& logFile) {

	logFile << " -> Detect gradual transitions (Based on ECR):" << endl;
	logFile << " max std dev = " << maxStdDev << endl;
	logFile << " min transition length = " << minLength << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

	int nrFrames = frames.size();
	Mat previousFrame, currentFrame;
	vector<FrameTransition> fades;

	vector<int> monochromes = findMonochromeFrames(frames, maxStdDev);
	int nrPotentialFades = monochromes.size();
	int prevTransitionStart = 0;
	int prevTransitionLength = 0;

	for (int i = 0; i < nrPotentialFades; i++) {
		int interm = monochromes[i];
		int start = interm, end = interm;
		Mat prevFrame, currFrame;
		float ecr_in=-1, ecr_out = 0;

		// in the first part of a dissolve transition ecr_out > ecr_in (more edges vanish), same for the fade-out transition
		prevFrame = frames[interm];
		start = interm - 1;
		while (start >= 0 && ecr_out > ecr_in) {
			currFrame = frames[start];
			computeECR(currFrame, prevFrame, ecr_in, ecr_out);
			prevFrame = currFrame;
			start--;
		}

		ecr_in = 0;
		ecr_out = -1;
		// in the second part of a dissolve transition ecr_out < ecr_in (more edges appear), same for the fade-in transition
		prevFrame = frames[interm];
		end = interm + 1;
		while (end < nrFrames && ecr_out < ecr_in) {
			currFrame = frames[end];
			computeECR(prevFrame, currFrame, ecr_in, ecr_out);
			prevFrame = currFrame;
			end++;
		} 

		if (abs(interm - start) >= minLength) {
			// fade-out
			end = interm;
			start = start + 1;
			if (start > prevTransitionStart) {
				// do not overlap with previous fade-out transition
				FrameTransition ft = { start, end, FADE_OUT };
				fades.push_back(ft);
				logFile << "fade out frames[" << start << ", " << end << "]" << endl;
				prevTransitionStart = start;
				prevTransitionLength = end - start;
			}
		}
		else if (abs(end - interm) >= minLength) {
			// fade-in 
			start = interm;
			end = end - 1;
			if (start > prevTransitionStart + prevTransitionLength) {
				// do not overlap with previous fade-in transition
				FrameTransition ft = { start, end, FADE_IN };
				fades.push_back(ft);
				logFile << "fade in frames[" << start << ", " << end << "]" << endl;
				prevTransitionStart = start;
				prevTransitionLength = end - start;
			}
		}
	}
	logFile << " --------------------------------------------------------------------------------------" << endl;

	return fades;
}