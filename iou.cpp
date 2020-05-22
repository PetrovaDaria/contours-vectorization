//
// Created by Dariya Petrova on 21.05.2020.
//

#include "iou.h"

double getIou(Mat img1, Mat img2) {
    Mat gray1;
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    Mat bwimg1;
    threshold(gray1, bwimg1, 127, 255,THRESH_BINARY);
    int area1 = getAllContoursArea(bwimg1);

    //aaaaaaaaa
    Mat gray2;
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    Mat bwimg2;
    threshold(img2, bwimg2, 127, 255,THRESH_BINARY);
    int area2 = getAllContoursArea(bwimg2);

    Mat intersectionImg;
    bitwise_and(img1, img2, intersectionImg);
    Mat bwimgInter;
    threshold(intersectionImg, bwimgInter, 127, 255,THRESH_BINARY);
    int intersectionArea = getAllContoursArea(bwimgInter);
    int joinArea = area1 + area2 - intersectionArea;
    double iou = (double)intersectionArea / joinArea;
    return iou;
}

int getAllContoursArea(Mat img) {
    int count = countNonZero(img);
    return count;
}
