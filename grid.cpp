//
// Created by Dariya Petrova on 11.05.2020.
//

#include "drawing.h"

#include "grid.h"
using namespace cv;
using namespace std;

void gribovAlgorithm() {
    Mat img = imread("../oneBuilding.png");
    cout << img.size() << endl;

    Mat contoursImg;
    Canny(img, contoursImg, 100, 255);
    imshow("Contours", contoursImg);
    imwrite("../oneBuildingGrid.jpg", contoursImg);

    vector<vector<Point>> contours;
    findContours(contoursImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    cout << contours[0].size() << endl;

    vector<Point> dpContour;
//        tuple<vector<Point>, bool> singleResult = contourUtils::doubleContourToSingle(dpContour);
//        cout << "is single " << get<1>(singleResult);
//        dpContour = get<0>(singleResult);
    // dpContour = myDp::ramerDouglasPeuckerRecr(contours[0], 2);
    approxPolyDP(contours[0], dpContour, 2, true);
    printPoints(dpContour);

    cout << dpContour.size() << endl;

    Mat gridImg = Mat::zeros(img.size(), CV_8UC3);
    drawLines(gridImg, dpContour, Scalar(0, 255, 0));
    showImg(gridImg, "dp contour");
}
