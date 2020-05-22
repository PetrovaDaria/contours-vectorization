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

using namespace cv;
using namespace std;

int main();
void processing();
void iterProcessing();
bool canBeDescribedByRect(vector<Point> contour, int maxDiff);
vector<Point> processingMinAreaRect(vector<Point> contour);

