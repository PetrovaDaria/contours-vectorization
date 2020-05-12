//
// Created by Dariya Petrova on 12.05.2020.
//

using namespace cv;
using namespace std;

#pragma once
void first();
void convexHull();
void contours();
void rotatedMinAreaRect();
void projection();
void test();
void integratedDP();

// grid
//void gribovAlgorithm();

// my dp
double perpendicularLength(Point first, Point second, Point zero);
vector<Point> ramerDouglasPeuckerRecr(vector<Point> polyline, double eps);
vector<Point> douglasPeucker(vector<Point> polyline, double eps);
