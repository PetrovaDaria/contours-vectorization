//
// Created by Dariya Petrova on 11.05.2020.
//

#include <iostream>
#include "grid.h"

Scalar red2(Scalar(0, 0, 255));
Scalar green2(Scalar(0, 255, 0));
Scalar blue2(Scalar(255, 0, 0));

// пока что на примере картинки с одним контуром реализую алгоритм Грибова
void gribovAlgorithm() {
    Mat img = imread("../oneBuilding.jpeg");

    Mat contoursImg;
    Canny(img, contoursImg, 100, 255);
    imshow("Contours", contoursImg);
    imwrite("../oneBuildingGrid.jpg", contoursImg);

    vector<vector<Point>> contours;
    findContours(contoursImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<Point> dpContour;
    approxPolyDP(contours[0], dpContour, 3, true);

    Mat gridImg = Mat::zeros(img.size(), CV_8UC3);
    Mat rotatedContourImg = Mat::zeros(img.size(), CV_8UC3);

    int gridStartX = 0;
    int gridStartY = 0;
    int gridIntervalX = 3;
    int gridIntervalY = 3;

    drawGrid(gridStartX, gridStartY, gridIntervalX, gridIntervalY, gridImg, blue2);
    drawLines(gridImg, dpContour, green2);
    drawPoints(gridImg, dpContour, red2);
    showImg(gridImg, "dp contour");

    double rotationAngle = getRotationAngleInDeg(dpContour);

    Point centroid = getCentroidPoint(dpContour);

    vector<Point> rotatedContour;

    for (Point point: dpContour) {
        Point rotatedPoint = getRotatedPoint(point, centroid, rotationAngle);
        rotatedContour.push_back(rotatedPoint);
        Point nearestGridPoint = getNearestGridPoint(rotatedPoint, gridStartX, gridStartY, gridIntervalX, gridIntervalY);
        vector<Point> neighborPoints = getAuxilaryGridPoints(nearestGridPoint, gridIntervalX, gridIntervalY);
        drawPoints(rotatedContourImg, neighborPoints, blue2);
    }
    drawLines(rotatedContourImg, dpContour, green2);
    drawLines(rotatedContourImg, rotatedContour, red2);
    drawPoints(rotatedContourImg, rotatedContour, Scalar(0, 255, 255));
    showImg(rotatedContourImg, "rotated");
}

void drawGrid(int startX, int startY, int intervalX, int intervalY, Mat img, Scalar color) {
    int imgWidth = img.cols;
    int imgHeight = img.rows;
    vector<Point> gridPoints;
    for (int x = startX; x < imgWidth; x += intervalX) {
        for (int y = startY; y < imgHeight; y += intervalY) {
            gridPoints.push_back(Point(x, y));
        }
    }
    drawPoints(img, gridPoints, color);
}

double getRotationAngleInDeg(vector<Point> contour) {
    RotatedRect rect = minAreaRect(contour);
    return rect.angle;
}

Point getCentroidPoint(vector<Point> contour) {
    Moments cntMoments = moments(contour);
    double centerX = cntMoments.m10 / cntMoments.m00;
    double centerY = cntMoments.m01 / cntMoments.m00;
    return Point(centerX, centerY);
}

double getPolarAngle(Point point) {
    return atan2(point.y,point.x);
}

double getPolarRadius(Point point) {
    return sqrt(point.x * point.x + point.y * point.y);
}

double fromDegToRad(double angleInDeg) {
    return angleInDeg / 180 * M_PI;
}

double fromRadToDeg(double angleInRad) {
    return angleInRad * 180 / M_PI;
}

Point fromPolarToDecart(double angleInRad, double radius) {
    int x = radius * cos(angleInRad);
    int y = radius * sin(angleInRad);
    return Point(x, y);
}

Point getNearestGridPoint(Point point, int startX, int startY, int intervalX, int intervalY) {
    int nearestX = getNearestCoord(point.x, startX, intervalX);
    int nearestY = getNearestCoord(point.y, startY, intervalY);
    return Point(nearestX, nearestY);
}

int getNearestCoord(int pointCoord, int gridStart, int gridInterval) {
    int dist = (pointCoord - gridStart) % gridInterval;
    if (dist < gridInterval / 2.0) {
        return pointCoord - dist;
    }
    return pointCoord + (gridInterval - dist);
}

vector<Point> getAuxilaryGridPoints(Point centerPoint, int intervalX, int intervalY) {
    vector<Point> neighborPoints;
    neighborPoints.push_back(centerPoint);
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            neighborPoints.push_back(Point(centerPoint.x + i * intervalX, centerPoint.y + j * intervalY));
        }
    }
    return neighborPoints;
}

Point getRotatedPoint(Point point, Point centroid, double rotationAngle) {
    Point centeredPoint = point - centroid;
    double phi = getPolarAngle(centeredPoint);
    double rho = getPolarRadius(centeredPoint);
    double phiInDeg = fromRadToDeg(phi);
    double newAngleInDeg = phiInDeg - rotationAngle;
    double newAngleInRad = fromDegToRad(newAngleInDeg);
    Point rotatedPoint = fromPolarToDecart(newAngleInRad, rho);
    Point inPlacePoint = rotatedPoint + centroid;
    return inPlacePoint;
}

double getPCAAngle() {
    //    // поворот
//    Mat pts_mat(dpContour.size(), 2, CV_64FC1);
//    for (int j = 0; j < pts_mat.rows; ++j) {
//        pts_mat.at<double>(j,0) = dpContour[j].x;
//        pts_mat.at<double>(j,1) = dpContour[j].y;
//    }
//
//    PCA pca(pts_mat, Mat(), PCA::DATA_AS_ROW);
//
//    // В отдельный вектор выносим собственные вектора,
//    // полученные при помощи PCA.
//    vector<Point2d> eigen_vecs(2);
//    vector<double> eigen_val(2);
//    for(int j = 0; j < 2; ++j) {
//        eigen_vecs[j] = Point2d(pca.eigenvectors.at<double>(j,0), pca.eigenvectors.at<double>(j,1));
//        eigen_val[j] = pca.eigenvalues.at<double>(0, j);
//    }
//
//    Point pos = Point(pca.mean.at<double>(0, 0), pca.mean.at<double>(0, 1));
//    line(rotatedContourImg, pos, pos + 0.02 * Point(eigen_vecs[0].x * eigen_val[0], eigen_vecs[0].y * eigen_val[0]) , CV_RGB(255, 255, 0));
//    line(rotatedContourImg, pos, pos + 0.02 * Point(eigen_vecs[1].x * eigen_val[1], eigen_vecs[1].y * eigen_val[1]) , CV_RGB(0, 255, 255));
//
//    drawLines(rotatedContourImg, dpContour, green);
//
//    // Искомый угол.
//    double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x);
//    cout << angle << endl;
}
