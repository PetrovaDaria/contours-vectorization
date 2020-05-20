//
// Created by Dariya Petrova on 18.05.2020.
//

#include <iostream>
#include <vector>
#include <fstream>
#include "projection.h"

extern Scalar myRed;
extern Scalar myGreen;
extern Scalar myBlue;
extern Scalar myPink;

void projection() {
    String parametersPath = "../projectionParameters.txt";
    String inputPath = "../oneBuilding4.jpg";
    String outputPath = "../projection4.jpg";

    //
    ofstream out;
    out.open(parametersPath);
    //

    Mat img = imread(inputPath);

    //
    out << img.cols << endl;
    out << img.rows << endl;
    //

    Mat contoursImg;
    Canny(img, contoursImg, 100, 255);

    vector<vector<Point>> contours;
    findContours(contoursImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    //
    out << contours.size() << endl;
    //

    vector<Point> contour  = contours[0];

    //
    out << contour.size() << endl;
    //

    for (Point point: contour) {
        out << point.x << " " << point.y << endl;
    }

    vector<Point> dpContour;
    approxPolyDP(contour, dpContour, 3, true);

    Mat projectionImg = Mat::zeros(img.size(), CV_8UC3);
    drawLines(projectionImg, dpContour, myGreen);
    drawPoints(projectionImg, dpContour, myRed);

    int n = dpContour.size();

    int longestSegmentNum = getLongestSegment(dpContour);

    int startIndex = longestSegmentNum;
    int endIndex = (startIndex + 1) % n;
    int nextIndex = (startIndex + 2) % n;

    double rotationAngle = getRotationAngleInDeg(dpContour);
    Point initCentroid = getCentroidPoint(dpContour);
    vector<Point> rotatedContour = rotateContour(dpContour, rotationAngle);

    vector<Point> newContour;
    for (Point point: rotatedContour) {
        newContour.push_back(point);
    }

    for (int i = 0; i < n; i++) {
        Point proj = getProjection(newContour[startIndex], newContour[endIndex], newContour[nextIndex]);
        newContour[endIndex] = proj;
        startIndex = (startIndex + 1) % n;
        endIndex = (endIndex + 1) % n;
        nextIndex = (nextIndex + 1) % n;
    }

    vector<Point> newContourRotated = rotateContourWithCentroid(newContour, -rotationAngle, initCentroid);

    //
    out << newContourRotated.size() << endl;

    for (Point point: newContourRotated) {
        out << point.x << " " << point.y << endl;
    }

    out.close();
    //

    drawLines(projectionImg, newContourRotated, myPink);

    showImg(projectionImg, "Projection");
    imwrite(outputPath, projectionImg);
}

double getSegmentLength(Point start, Point end) {
    return sqrt(pow((end.x - start.x), 2) + pow((end.y - start.y), 2));
}

// возвращает номер сегмента. соответственно номера точек образующих сегмент i и i+1
int getLongestSegment(vector<Point> contour) {
    int n = contour.size();
    double maxLength = 0;
    int longestSegmentNum;
    for (int i = 1; i <= n; i++) {
        int startIndex = i - 1;
        int endIndex = i % n;
        Point start = contour[startIndex];
        Point end = contour[endIndex];
        double length = getSegmentLength(start, end);
        if (length > maxLength) {
            maxLength = length;
            longestSegmentNum = startIndex;
        }
    }
    return longestSegmentNum;
}

Point getProjection(Point startPoint, Point endPoint, Point toProject) {
    if (endPoint.y == startPoint.y) {
        return Point(toProject.x, startPoint.y);
    }
    double a = endPoint.y - startPoint.y;
    double b = - endPoint.x + startPoint.x;
    double c = startPoint.x * a + startPoint.y * b;
    double d = (endPoint.x - startPoint.x) / (endPoint.y - startPoint.y);
    double e = 1;
    double f = toProject.x * d + toProject.y;
    double mainDet =  a * e - b * d;
    double xDet = c * e - b * f;
    double yDet = a * f - c * d;
    double x = xDet / mainDet;
    double y = yDet / mainDet;
    return Point(x, y);
}

