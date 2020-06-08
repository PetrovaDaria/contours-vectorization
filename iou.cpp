//
// Created by Dariya Petrova on 21.05.2020.
//

#include <iostream>
#include "iou.h"

/*!
 * вычиляет метрику IoU - intersection over union
 * @param img1 - первое изображение
 * @param img2 - второе изображение
 * @return значение iou
 */
double getIou(Mat img1, Mat img2) {
    Mat gray1;
    cvtColor(img1, gray1, COLOR_BGR2GRAY);
    Mat blWhImg1;
    threshold(gray1, blWhImg1, 127, 255, THRESH_BINARY);

    Mat gray2;
    cvtColor(img2, gray2, COLOR_BGR2GRAY);
    Mat blWhImg2;
    threshold(img2, blWhImg2, 127, 255, THRESH_BINARY);

    Mat intersectionImg;
    bitwise_and(img1, img2, intersectionImg);

    Mat gray3;
    cvtColor(intersectionImg, gray3, COLOR_BGR2GRAY);
    Mat blWhImgInter;
    threshold(gray3, blWhImgInter, 127, 255, THRESH_BINARY);
    int intersectionArea = countNonZero(blWhImgInter);

    Mat joinImg;
    bitwise_or(img1, img2, joinImg);

    Mat gray4;
    cvtColor(joinImg, gray4, COLOR_BGR2GRAY);
    Mat blWhImgJoin;
    threshold(gray4, blWhImgJoin, 127, 255, THRESH_BINARY);
    int joinArea = countNonZero(blWhImgJoin);

    double iou = (double)intersectionArea / joinArea;
    return iou;
}

