//
// Created by Dariya Petrova on 11.05.2020.
//

#include <iostream>
#include <map>
#include <unordered_map>
#include "grid.h"

extern Scalar red;
extern Scalar green;
extern Scalar blue;

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

    // тут просто по фану рисуется сетка
//    drawGrid(gridStartX, gridStartY, gridIntervalX, gridIntervalY, gridImg, blue2);
//    drawLines(gridImg, dpContour, green2);
//    drawPoints(gridImg, dpContour, red2);
//    showImg(gridImg, "dp contour");

    double rotationAngle = getRotationAngleInDeg(dpContour);

    Point centroid = getCentroidPoint(dpContour);

    vector<Point> rotatedContour;

    map<Point, vector<Point>, comparePoints> auxilaryPoints;

    for (const Point& point: dpContour) {
        Point rotatedPoint = getRotatedPoint(point, centroid, rotationAngle);
        rotatedContour.push_back(rotatedPoint);
        Point nearestGridPoint = getNearestGridPoint(rotatedPoint, gridStartX, gridStartY, gridIntervalX, gridIntervalY);
        vector<Point> neighborPoints = getAuxilaryGridPoints(nearestGridPoint, gridIntervalX, gridIntervalY);
        auxilaryPoints[rotatedPoint] = neighborPoints;
        drawPoints(rotatedContourImg, neighborPoints, blue);
    }
    drawLines(rotatedContourImg, dpContour, green);
    drawLines(rotatedContourImg, rotatedContour, red);
    drawPoints(rotatedContourImg, rotatedContour, Scalar(0, 255, 255));

    vector<vector<int>> sc;
    vector<vector<int>> da;
    vector<vector<pair<int, int>>> bpp;

    int n = rotatedContour.size();
    int auxCount = 9;

    vector<int> tempSC;
    vector<int> tempDA;
    vector<pair<int, int>> tempBPP;
    for (int i = 0; i < auxCount; i++) {
        tempSC.push_back(0);
        tempDA.push_back(0);
        tempBPP.push_back(make_pair(0,0));
    }
    sc.push_back(tempSC);
    da.push_back(tempDA);
    bpp.push_back(tempBPP);

    int prevPointsCount = 3;

    for (int pointNum = 1; pointNum <= n; pointNum++) {
        Point currentPoint = rotatedContour[pointNum];
        tempSC.clear();
        tempDA.clear();
        tempBPP.clear();
        for (int auxNum = 0; auxNum < auxCount; auxNum++) {
            double minArea = INFINITY;
            int newSC = 0;
            pair<int, int> bestPoint;
            Point currentAuxPoint = auxilaryPoints[currentPoint][auxNum];
            for (int prevPointNum = pointNum - 1; prevPointNum >= 0 && prevPointNum > pointNum - prevPointsCount; prevPointNum--) {
                Point prevPoint = rotatedContour[prevPointNum];
                for (int prevAuxNum = 0; prevAuxNum < auxCount; prevAuxNum++) {
                    Point prevAuxPoint = auxilaryPoints[prevPoint][prevAuxNum];
                    if (prevPointNum != 0) {
                        Point prevPrevPoint = getPrevPoint(prevPointNum, prevAuxNum, rotatedContour, bpp, auxilaryPoints);
                        double angle = getAngleBetweenSegments(prevPrevPoint, prevAuxPoint, currentAuxPoint);
                        if ((int)angle % 45 != 0) {
                            continue;
                        }
                    }
                    vector<Point> newContour;
                    newContour.push_back(currentPoint);
                    for (int j = pointNum - 1; j > prevPointNum; j--){
                        Point newPoint = rotatedContour[j];
                        newContour.push_back(newPoint);
                    }
                    newContour.push_back(prevPoint);
                    newContour.push_back(prevAuxPoint);
                    newContour.push_back(currentAuxPoint);
                    double currentArea = getArea(newContour);
                    double allArea = currentArea + da[prevPointNum][prevAuxNum];
                    if (allArea < minArea) {
                        minArea = allArea;
                        bestPoint = make_pair(prevPointNum, prevAuxNum);
                        newSC = sc[prevPointNum][prevAuxNum] + 1;
                    }
                }
            }
            tempSC.push_back(newSC);
            tempDA.push_back(minArea);
            tempBPP.push_back(bestPoint);
        }
        sc.push_back(tempSC);
        da.push_back(tempDA);
        bpp.push_back(tempBPP);
    }

    // тут нужно соединить последнее и первое

    int lastIndex = rotatedContour.size() - 1;

    int minSC = INFINITY;
    int almostMinSC = INFINITY;
    vector<int> minAuxIndex;
    vector<int> almostMinIndex;

    for (int i = 0; i < auxCount; i++) {
        int currentSC = sc[lastIndex][i];
        if (currentSC <= minSC) {
            if (currentSC == minSC) {
                minAuxIndex.push_back(i);
            }
            if (currentSC < minSC) {
                almostMinSC = minSC;
                almostMinIndex = minAuxIndex;

                minSC = currentSC;
                minAuxIndex.clear();
                minAuxIndex.push_back(i);
            }
        } else {
            if (currentSC < almostMinSC) {
                almostMinSC = currentSC;
                almostMinIndex.clear();
                almostMinIndex.push_back(i);
            }
            if (currentSC == almostMinSC) {
                almostMinIndex.push_back(i);
            }
        }
    }

    int bestAuxIndex;

    if (minAuxIndex.size() > 1) {
        double minArea = INFINITY;
        for (int i: minAuxIndex) {
            int currentArea = da[lastIndex][i];
            if (currentArea < minArea) {
                minArea = currentArea;
                bestAuxIndex = i;
            }
        }
    } else {
        bestAuxIndex = minAuxIndex[0];
    }

    vector<Point> rightContour;

    int currentPointIndex = lastIndex;
    int currentAuxIndex = bestAuxIndex;

    while (currentPointIndex > 0) {
        if (currentPointIndex == 0) {

        }
        Point currentPoint = auxilaryPoints[rotatedContour[currentPointIndex]][currentAuxIndex];
        rightContour.push_back(currentPoint);
        pair<int, int> prevCoord = bpp[currentPointIndex][currentAuxIndex];
        currentPointIndex = prevCoord.first;
        currentAuxIndex = prevCoord.second;
    }

    drawLines(rotatedContourImg, rightContour, Scalar(255, 0, 255));

//    double minArea = 10000000;
//    pair<Point, Point> bestPair;
//    for (int j = 0; j < 9; j++) {
//        for (int k = 0; k < 9; k++) {
//            Mat cloned = rotatedContourImg.clone();
//            vector<Point> newContour;
//            newContour.push_back(auxilaryPoints[rotatedContour[0]][j]);
//            newContour.push_back(auxilaryPoints[rotatedContour[1]][k]);
//            newContour.push_back(rotatedContour[1]);
//            newContour.push_back(rotatedContour[0]);
//            double newContourArea = contourArea(newContour);
//            if (newContourArea < minArea) {
//                minArea = newContourArea;
//                bestPair = make_pair(auxilaryPoints[rotatedContour[0]][j], auxilaryPoints[rotatedContour[1]][k]);
//            }
//            cout << auxilaryPoints[rotatedContour[0]][j] << " " << auxilaryPoints[rotatedContour[1]][k] << " " << newContourArea << endl;
//            line(cloned, auxilaryPoints[rotatedContour[0]][j], auxilaryPoints[rotatedContour[1]][k], Scalar(255, 0, 255));
//            RotatedRect rect = minAreaRect(newContour);
//            Point2f rectPoints[4];
//            rect.points(rectPoints);
//            for (int l = 0; l < 4; l++ )
//            {
//                line(cloned, rectPoints[l], rectPoints[(l+1)%4], Scalar(0, 255, 255));
//            }
//            // drawLines(cloned, newContour, Scalar(255, 0, 255));
//            showImg(cloned, "rotated");
//        }
//    }
//
//    cout << "best " << bestPair.first << " " << bestPair.second << " " << minArea << endl;
//    line(rotatedContourImg, bestPair.first, bestPair.second, Scalar(255, 0, 255));
//    cout << getAngleBetweenSegments(rotatedContour[0], rotatedContour[1], rotatedContour[2]) << endl;
    showImg(rotatedContourImg, "rotated");
    imwrite("../result3.jpg", rotatedContourImg);
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

double getAngleBetweenSegments(Point point1, Point point2, Point point3) {
    Point q1 = Point(point2.x - point1.x, point2.y - point1.y);
    Point q2 = Point(point3.x - point2.x, point3.y - point2.y);
    double temp = (q1.x * q2.x + q1.y * q2.y) /
                  (sqrt(q1.x * q1.x+ q1.y * q1.y) * sqrt(q2.x * q2.x+ q2.y * q2.y));
    double angleInRad = acos(temp);
    return fromRadToDeg(angleInRad);
}

double getArea(vector<Point> contour) {
    //return contourArea(contour);
    cout << "Their area " << contourArea(contour) << endl;
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
    cout << "My area " << cntArea << endl;
    return cntArea;
}

Point getPrevPoint(int pointNum, int auxNum, vector<Point> contour,
        vector<vector<pair<int, int>>> bpp, map<Point, vector<Point>, comparePoints> auxilaryPoints) {
    int prevPointNum = bpp[pointNum][auxNum].first;
    int prevAuxPointNum = bpp[pointNum][auxNum].second;
    Point prevPoint = auxilaryPoints[contour[prevPointNum]][prevAuxPointNum];
    return prevPoint;
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
    return 0;
}
