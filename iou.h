//
// Created by Dariya Petrova on 21.05.2020.
//

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "contourUtils.h"
#include "drawing.h"

using namespace cv;
using namespace std;

double getIou(Mat img1, Mat img2);
