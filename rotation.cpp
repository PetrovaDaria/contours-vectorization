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

vector<Point> rotateContourWithCentroid(vector<Point> contour, double angle, Point centroid) {
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

// pca, результат работы которого мне не очень нравится
double getPCAAngle(vector<Point> contour) {
    Mat pts_mat(contour.size(), 2, CV_64F);
    for (int j = 0; j < pts_mat.rows; ++j) {
        pts_mat.at<double>(j,0) = contour[j].x;
        pts_mat.at<double>(j,1) = contour[j].y;
    }

    PCA pca(pts_mat, Mat(), PCA::DATA_AS_ROW);

    // В отдельный вектор выносим собственные вектора,
    // полученные при помощи PCA.
    vector<Point2d> eigen_vecs(2);
    vector<double> eigen_val(2);
    for(int j = 0; j < 2; j++) {
        eigen_vecs[j] = Point2d(pca.eigenvectors.at<double>(j,0), pca.eigenvectors.at<double>(j,1));
        eigen_val[j] = pca.eigenvalues.at<double>(0, j);
    }

    Point pos = Point(pca.mean.at<double>(0, 0), pca.mean.at<double>(0, 1));
//    line(rotatedContourImg, pos, pos + 0.02 * Point(eigen_vecs[0].x * eigen_val[0], eigen_vecs[0].y * eigen_val[0]) , CV_RGB(255, 255, 0));
//    line(rotatedContourImg, pos, pos + 0.02 * Point(eigen_vecs[1].x * eigen_val[1], eigen_vecs[1].y * eigen_val[1]) , CV_RGB(0, 255, 255));
//
//    drawLines(rotatedContourImg, dpContour, green);

    // Искомый угол.
    double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x);
    angle = fromRadToDeg(angle);
    return angle;
}
