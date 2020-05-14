//
// Created by Dariya Petrova on 11.05.2020.
//

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "drawing.h"

void gribovAlgorithm();
void drawGrid(int startX, int startY, int intervalX, int intervalY, Mat img, Scalar color);
double getRotationAngleInDeg(vector<Point> contour);
Point getCentroidPoint(vector<Point> contour);
double getPolarAngle(Point point);
double getPolarRadius(Point point);
double fromDegToRad(double angleInDeg);
double fromRadToDeg(double angleInRad);
Point fromPolarToDecart(double angleInRad, double radius);
Point getNearestGridPoint(Point point, int startX, int startY, int intervalX, int intervalY);
vector<Point> getAuxilaryGridPoints(Point centerPoint, int intervalX, int intervalY);
int getNearestCoord(int pointCoord, int gridInterval, int gridStart);
Point getRotatedPoint(Point point, Point centroid, double rotationAngle);
double getPCAAngle();
