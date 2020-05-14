//
// Created by Dariya Petrova on 11.05.2020.
//

#include <iostream>
#include "drawing.h"

void showImg(Mat img, String imgName) {
    imshow(imgName, img);
    waitKey(0);
}

void drawLines(Mat img, vector<Point> points, Scalar color, bool joinEnds = true, bool isShowImg = false,
               String imgName = "") {
    int size = points.size();
    int end = size;
    if (!joinEnds) {
        end -= 1;
    }
    for (int j = 0; j < end; j++) {
        line(img, points[j], points[(j + 1) % size], color);
        if (isShowImg) {
            showImg(img, imgName);
        }
    }
}

void drawPoints(Mat img, vector<Point> points, Scalar color) {
    int size = points.size();
    for (int j = 0; j < size; j++) {
        line(img, points[j], points[j], color);
    }
}

void printPoints(vector<Point> points) {
    for (Point point: points) {
        cout << point.x << ";" << point.y << endl;
    }
}
