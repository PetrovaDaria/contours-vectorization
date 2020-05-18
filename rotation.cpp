//
// Created by Dariya Petrova on 18.05.2020.
//

#include <vector>

#include "rotation.h"

vector<Point> rotateContour(vector<Point> contour, double angle) {
    Point centroid = getCentroidPoint(contour);
    vector<Point> rotatedContour;
    for (Point point: contour) {
        rotatedContour.push_back(getRotatedPoint(point, centroid, angle));
    }
    return rotatedContour;
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

double getRotationAngleInDeg(vector<Point> contour) {
    RotatedRect rect = minAreaRect(contour);
    return rect.angle;
}
