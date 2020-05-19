//
// Created by Dariya Petrova on 11.05.2020.
//

#include <iostream>
#include <map>
#include <unordered_map>
#include <fstream>
#include "grid.h"

extern Scalar red;
extern Scalar green;
extern Scalar blue;
extern Scalar pink;

// пока что на примере картинки с одним контуром реализую алгоритм Грибова
void gribovAlgorithm() {
    ofstream out;
    out.open("../gridParameters.txt");

    Mat img = imread("../oneBuilding2.jpg");

    out << img.cols << endl;
    out << img.rows << endl;

    Mat contoursImg;
    Canny(img, contoursImg, 100, 255);

    vector<vector<Point>> contours;
    findContours(contoursImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    out << contours.size() << endl;

    vector<Point> contour  = contours[0];

    out << contour.size() << endl;

    for (Point point: contour) {
        out << point.x << " " << point.y << endl;
    }

    vector<Point> dpContour;
    approxPolyDP(contour, dpContour, 3, true);

    Mat gridImg = Mat::zeros(img.size(), CV_8UC3);
    Mat rotatedContourImg = Mat::zeros(img.size(), CV_8UC3);

    drawLines(rotatedContourImg, dpContour, green);

    int gridStartX = 0;
    int gridStartY = 0;
    int gridIntervalX = 3;
    int gridIntervalY = 3;

    double rotationAngle = getRotationAngleInDeg(dpContour);

    vector<Point> rotatedContour = rotateContour(dpContour, rotationAngle);

    drawLines(rotatedContourImg, rotatedContour, red);

    map<Point, vector<Point>, comparePoints> auxilaryPoints;

    for (const Point& rotatedPoint: rotatedContour) {
        Point nearestGridPoint = getNearestGridPoint(rotatedPoint, gridStartX, gridStartY, gridIntervalX, gridIntervalY);
        vector<Point> neighborPoints = getAuxilaryGridPoints(nearestGridPoint, gridIntervalX, gridIntervalY);
        auxilaryPoints[rotatedPoint] = neighborPoints;
        drawPoints(rotatedContourImg, neighborPoints, blue);
    }

    drawPoints(rotatedContourImg, rotatedContour, Scalar(0, 255, 255));

    vector<vector<int>> sc;
    vector<vector<int>> da;
    // pair int-int - номер предыдущей точки контура и номер ее вспомогательной точки
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

    double angleTolerance = 2;

    for (int pointNum = 1; pointNum <= n; pointNum++) {
        int pointNumMod = pointNum % n; //
        // Point currentPoint = rotatedContour[pointNum];
        Point currentPoint = rotatedContour[pointNumMod];
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
                    if (prevPointNum == 0 && pointNum != 1) {
                        continue;
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
        if (pointNum == n) {
            sc[0] = tempSC;
            da[0] = tempDA;
            bpp[0] = tempBPP;
        } else {
            sc.push_back(tempSC);
            da.push_back(tempDA);
            bpp.push_back(tempBPP);
        }
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

    while (true) {
        Point currentPoint = auxilaryPoints[rotatedContour[currentPointIndex]][currentAuxIndex];
        rightContour.push_back(currentPoint);
        pair<int, int> prevCoord = bpp[currentPointIndex][currentAuxIndex];
        if (currentPointIndex == 0) {
            break;
        }
        currentPointIndex = prevCoord.first;
        currentAuxIndex = prevCoord.second;
    }

    drawLines(rotatedContourImg, rightContour, pink);

    vector<Point> rightRotatedContour = rotateContour(rightContour, -rotationAngle);

    out << rightRotatedContour.size() << endl;

    for (Point point: rightRotatedContour) {
        out << point.x << " " << point.y << endl;
    }

    drawLines(rotatedContourImg, rightRotatedContour, pink);
    showImg(rotatedContourImg, "rotated");
    imwrite("../grid2.jpg", rotatedContourImg);
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
    // cout << "Their area " << contourArea(contour) << endl;
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
    // cout << "My area " << cntArea << endl;
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
