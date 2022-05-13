#pragma once
#include "stdafx.h"
#include <fstream>
#include <ctime>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include "Utils.h"
#include "GradualTransitionDetector.h"

vector<Mat> readAllFrames(const char* fileName);
void saveAllFrames(vector<Mat> allFrames, char* videoFilePath, char* outputDirPath);
void saveKeyFrames(vector<FrameTransition> keyFrames, vector<Mat> allFrames, char* outputDirPath);