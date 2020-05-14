//
// Created by Dariya Petrova on 11.05.2020.
//

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

double perpendicularLength(Point first, Point second, Point zero);
vector<Point> ramerDouglasPeuckerRecr(vector<Point> polyline, double eps);
vector<Point> douglasPeucker(vector<Point> polyline, double eps);
