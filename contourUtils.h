//
// Created by Dariya Petrova on 11.05.2020.
//

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int getContourArea(vector<Point> contour);
tuple<vector<Point>, bool> doubleContourToSingle(vector<Point> points);
vector<Point> deleteRepeatedNeighborPoints(vector<Point> points);
