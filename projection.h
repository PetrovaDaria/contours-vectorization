//
// Created by Dariya Petrova on 18.05.2020.
//

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "drawing.h"
#include "rotation.h"

using namespace std;

void projection();
double getSegmentLength(Point start, Point end);
int getLongestSegment(vector<Point> contour);
Point getProjection(Point startPoint, Point endPoint, Point toProject);
