//
// Created by Dariya Petrova on 12.05.2020.
//

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "contourUtils.cpp"
#include "drawing.cpp"
#include "grid.cpp"
#include "myDP.cpp"
#include "projection.cpp"
#include "rotation.cpp"
#include "iou.cpp"
#include "parameters.cpp"

using namespace cv;
using namespace std;

int main();
void tryDiffCnts();
void testGetArea();
vector<vector<Point>> getVectorizedContoursFromImg(Mat img, Parameters params, String outputImgPath);
vector<vector<Point>> getVectorizedContoursFromContours(vector<vector<Point>> contours, Parameters params);

void processing();
void iterProcessing(String dirPath, String inputImgPath, String markingImgPath);
