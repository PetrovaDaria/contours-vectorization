//
// Created by Dariya Petrova on 18.05.2020.
//

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

vector<Point> rotateContour(vector<Point> contour, double angle);
Point getRotatedPoint(Point point, Point centroid, double rotationAngle);
Point getCentroidPoint(vector<Point> contour);
double getPolarAngle(Point point);
double getPolarRadius(Point point);
double fromDegToRad(double angleInDeg);
double fromRadToDeg(double angleInRad);
Point fromPolarToDecart(double angleInRad, double radius);
double getRotationAngleInDeg(vector<Point> contour);
