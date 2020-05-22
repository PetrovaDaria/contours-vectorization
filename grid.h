//
// Created by Dariya Petrova on 11.05.2020.
//

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "contourUtils.h"
#include "drawing.h"
#include "rotation.h"

struct comparePoints {
    bool operator () (cv::Point const& a, cv::Point const& b) const
    {
        return (a.x < b.x) || (a.x == b.x && a.y < b.y);
    }
};

void gribovAlgorithm();
vector<Point> processingGribovAlgorithm(
        Mat img,
        vector<Point> contour,
        int dpEps,
        int gridStartX,
        int gridStartY,
        int gridIntervalX,
        int gridIntervalY,
        int prevPointsCount
);
Point getNearestGridPoint(Point point, int startX, int startY, int intervalX, int intervalY);
vector<Point> getAuxilaryGridPoints(Point centerPoint, int intervalX, int intervalY);
int getNearestCoord(int pointCoord, int gridInterval, int gridStart);
double getAngleBetweenSegments(Point point1, Point point2, Point point3);
Point getPrevPoint(int pointNum, int auxNum, vector<Point> contour,
                   vector<vector<pair<int, int>>> bpp, map<Point, vector<Point>, comparePoints> auxilaryPoints);
