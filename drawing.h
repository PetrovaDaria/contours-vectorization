//
// Created by Dariya Petrova on 12.05.2020.
//

#pragma once
void showImg(Mat img, String imgName);
void drawLines(Mat img, vector<Point> points, Scalar color, bool joinEnds = true, bool isShowImg = false,
               String imgName = "");
void drawPoints(Mat img, vector<Point> points, Scalar color);
void printPoints(vector<Point> points);
