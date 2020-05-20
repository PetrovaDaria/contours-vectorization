#include <iostream>

#include "main.h"


Scalar myRed(Scalar(0, 0, 255));
Scalar myGreen(Scalar(0, 255, 0));
Scalar myBlue(Scalar(255, 0, 0));
Scalar myPink(Scalar(255, 0, 255));

int main() {
//    vector<Point> first = {Point(1, 1), Point(1, 3), Point(4, 3), Point(4, 1)};
//    vector<Point> second = {Point(2, 2), Point(2, 6), Point(7, 6), Point(7, 2)};
//    double area = intersect(first, second);
//    cout << area << endl;
    // projection();
    gribovAlgorithm();
    // rotatedMinAreaRect();
    // !!! projection();
    // integratedDP();
}

void rotatedMinAreaRect() {
    Mat img = imread("../satimg.jpg");
    Mat grayImg, cannyOutput;
    cvtColor(img, grayImg, COLOR_BGR2GRAY);
    Canny(grayImg, cannyOutput, 100, 255);
    vector<vector<Point>> contours;
    findContours(cannyOutput, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<vector<Point>> hull(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        convexHull(contours[i], hull[i]);
    }
    Mat drawing = Mat::zeros(cannyOutput.size(), CV_8UC3);
    Scalar contourColor = Scalar(0, 255, 0);
    Scalar hullColor = Scalar(255, 0, 0);
    Scalar rectColor = Scalar(0, 0, 255);
    cout << contours.size();
    for (size_t i = 0; i < contours.size(); i++) {
        vector<Point> contour = contours[i];
        RotatedRect minRect = minAreaRect(contour);
        Rect boundRect = boundingRect(contour);
        int left = boundRect.x;
        int top = boundRect.y;
        int width = boundRect.width;
        int height = boundRect.height;
        int x_end = left + width;
        int y_end = top + height;
        int cntArea = 0;
        for (int x = left; x < x_end; x++)
        {
            for (int y = top; y < y_end; y++)
            {
                double test = pointPolygonTest(contour, Point2f(x, y), false);
                if (test == 1 || test == 0) {
                    cntArea += 1;
                }
            }
        }
        cout << i << endl;
        cout << "Contour size " << contour.size() << endl;
        cout << "Contour moment area " << moments(contour).m00 << endl;
        cout << "Contour area " << cntArea << endl;
        cout << "Rect size " << minRect.size << endl;
        cout << "Rect area " << minRect.size.area() << endl;
        Point2f rectPoints[4];
        minRect.points(rectPoints);
        drawContours(drawing, contours, (int)i, contourColor);
        for ( int j = 0; j < 4; j++ )
        {
            line( drawing, rectPoints[j], rectPoints[(j+1)%4], rectColor );
        }
        vector<Point> poly;
        approxPolyDP(contour, poly, 5, true);
        polylines(drawing, poly, true, Scalar(255, 0, 255), 1);
    }
    namedWindow("Min area rect", WINDOW_NORMAL);
    imshow("Min area rect", drawing);
    imwrite("../minAreaRect.jpg", drawing);
    waitKey(0);
}

void convexHull() {
    Mat img = imread("../satimg.jpg");
    Mat grayImg, cannyOutput;
    cvtColor(img, grayImg, COLOR_BGR2GRAY);
    Canny(grayImg, cannyOutput, 100, 255);
    vector<vector<Point>> contours;
    findContours(cannyOutput, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    vector<vector<Point>> hull(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        convexHull(contours[i], hull[i]);
    }
    Mat drawing = Mat::zeros(cannyOutput.size(), CV_8UC3);
    Scalar contourColor = Scalar(0, 0, 255);
    Scalar hullColor = Scalar(0, 255, 0);
    Scalar rectColor = Scalar(255, 0, 0);
    cout << contours.size();
    for (size_t i = 0; i < contours.size(); i++) {
        Rect boundRect = boundingRect(contours[i]);
        rectangle(drawing, boundRect.tl(), boundRect.br(), rectColor, 2);
        drawContours(drawing, contours, (int)i, contourColor);
        drawContours(drawing, hull, (int)i, hullColor);
    }
    imshow("Convex hull", drawing);
    waitKey(0);
}
