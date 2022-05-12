#include "stdafx.h"
#include "GradualTransitionDetector.h"
#include "StatisticalProperties.h"
#include "EBAalgorithms.h"

using namespace std;

vector<int> findMonochromeFrames(vector<Mat> frames, float T, int M) {
	vector<int> monochrome;
	int n = frames.size();
	for (int i = M; i < n-M; i+=M) {
		float minDev = INT_MAX;
		int minIdx = -1;
		for (int j = i - M; j < i + M; j++) {
			Vec3f stdDev = computeStandardDeviationColor(frames[j]);
			if (stdDev[0] < T && stdDev[1] < T && stdDev[2] < T) {
				float totalDev = stdDev[0] + stdDev[1] + stdDev[2];
				if (totalDev < minDev) {
					minDev = totalDev;
					minIdx = j;
				}
			}
		}
		if (minIdx >= 0) {
			monochrome.push_back(minIdx);
		}
	}
	// do not allow duplicates (overlapping windows)
	// sort ascending and select only the best ones at relatively big distance (M should be large enough to include only 1 fade transition)
	return monochrome;
}


vector<GradualTransition> detectFadeTransitions_v1(vector<Mat> frames, float maxStdDev, float maxChange, int minLength, ofstream& logFile) {
	
	logFile << " -> Detect fade in/out (Based on Mean and Standard Deviation):"<<endl;
	logFile << " max std dev = " << maxStdDev << endl;
	logFile << " max intensity change = " << maxChange << endl;
	logFile << " min transition length = " << minLength << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

	int nrFrames = frames.size();
	Mat previousFrame, currentFrame;
	vector<GradualTransition> fades;

	int M = nrFrames/10; // size of sliding window (10% of total frames)
	vector<int> monochromes = findMonochromeFrames(frames, maxStdDev,M);
	int nrPotentialFades = monochromes.size();

	for (int i = 0; i < nrPotentialFades; i++) {
		logFile << "monochrome #" << monochromes[i] << endl;

		// detect fade-in (the new image gradually appears) => monotonically increasing luminance

		int start = monochromes[i];
		int end = start;
		Vec3f prevMean = computeMeanIntensitiyValueColor(frames[start]);
		Vec3f currMean = prevMean;
		for (int u = start + 1; u < nrFrames; u++) {
			currMean = computeMeanIntensitiyValueColor(frames[u]);
			float diff_r = prevMean[0] - currMean[0];
			float diff_g = prevMean[1] - currMean[1];
			float diff_b = prevMean[2] - currMean[2];
			if ((diff_r <= 0 || diff_g <= 0 || diff_b <= 0) || (diff_r > maxChange || diff_g > maxChange || diff_b > maxChange)) {
				// if the luminance doesn't change monotonically anymore or the change in luminance is not gradual, then stop and mark the end of the fade transition
				end = u;
				break;
			}
			prevMean = currMean;
		}

		if ((end - start) >= minLength) {
			GradualTransition gt = { start, end, FADE_IN };
			fades.push_back(gt);
			logFile << "fade in frames [" << start << ", " << end << "]" << endl;
		}

		// detect fade-out (the image gradually disappears) => monotonically decreasing luminance

		start = monochromes[i];
		end = start;
		prevMean = computeMeanIntensitiyValueColor(frames[start]);
		currMean = prevMean;
		for (int u = start - 1; u >= 0; u--) {
			currMean = computeMeanIntensitiyValue(frames[u]);
			float diff_r = prevMean[0] - currMean[0];
			float diff_g = prevMean[1] - currMean[1];
			float diff_b = prevMean[2] - currMean[2];
			if ((diff_r <= 0 || diff_g <= 0 || diff_b <= 0) || (diff_r > maxChange || diff_g > maxChange || diff_b > maxChange)) {
				// if the luminance doesn't change monotonically anymore or the change in luminance is not gradual, then stop and mark the end of the fade transition
				end = u;
				break;
			}
			prevMean = currMean;
		}

		if ((end - start) >= minLength) {
			GradualTransition gt = { start, end, FADE_OUT };
			fades.push_back(gt);
			logFile << "fade out frames [" << start << ", " << end << "]" << endl;
		}
	}
	logFile << " --------------------------------------------------------------------------------------" << endl;

	return fades;
}


vector<GradualTransition> detectFadeTransitions_v2(vector<Mat> frames, float T, int minLength, ofstream& logFile) {

	logFile << " -> Detect fade in/out (Based on ECR):"<<endl;
	logFile << " threshold for ecr = " << T << endl;
	logFile << " min transition length = " << minLength << endl;
	logFile << " --------------------------------------------------------------------------------------" << endl;

	Mat previousFrame, currentFrame;
	vector<GradualTransition> fades;
	int nrFrames = frames.size();

	for(int i=0; i<nrFrames;i++) {
		float ecr_in = 0, ecr_out = 0, ecr = 0;

		// check for fade out transition
		int start = i;
		EffectType effect;
		do {
			i++;
			// first half of fade out
			computeECR(frames[i - 1], frames[i], ecr_in, ecr_out);
			ecr = max(ecr_in, ecr_out);
			if (ecr_out > ecr_in) {
				effect = FADE_OUT;
			}
			else if(ecr_in > ecr_out) {
				effect = FADE_IN;
			}
		} while (ecr > T);
		int end = i;

		if ((end - start) >= minLength) {
			GradualTransition gt = { start, end, effect };
			fades.push_back(gt);
			logFile << effectToString(effect) << " frames [" << start << ", " << end << "]" << endl;
			continue;
		}
	}
	logFile << " --------------------------------------------------------------------------------------" << endl;

	return fades;
}