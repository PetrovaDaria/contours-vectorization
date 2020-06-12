//
// Created by Dariya Petrova on 12.05.2020.
//

#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

void showImg(cv::Mat img, cv::String imgName);
void drawLines(cv::Mat img, std::vector<cv::Point> points, cv::Scalar color, bool joinEnds = true, bool isShowImg = false,
               String imgName = "");
void drawPoints(cv::Mat img, std::vector<cv::Point> points, cv::Scalar color);
void cropImg(String imgPath, String newImgPath, int startX, int startY, int width, int height);
void printPoints(std::vector<cv::Point> points);
