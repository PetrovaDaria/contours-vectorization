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
vector<vector<Point>> vectorizeImg(const Mat& img, const Mat& markedImg, int dpEps, int gridInterval, int ppc, const String& dirPath);
vector<vector<Point>> vectorizeImgWithSeveralParameters(
        const Mat& img,
        const Mat& markedImg,
        const vector<int>& dpEpss,
        const vector<int>& gridIntervals,
        const vector<int>& ppcs,
        const String& dirPath
);
