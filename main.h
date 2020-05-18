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

using namespace cv;
using namespace std;

int main();
double getSegmentLength(Point start, Point end);
Point getIntersectionOfLineAndPointPerpendicular(Point lineStart, Point lineEnd, Point point);
vector<double> getSidesLengths(vector<Point> points, bool joinEnds);
void integratedDP();
void projection();
void rotatedMinAreaRect();
void convexHull();
void contours();
