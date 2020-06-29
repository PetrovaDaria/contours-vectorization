//
// Created by Dariya Petrova on 11.05.2020.
//

#include <iostream>
#include <map>
#include <unordered_map>
#include <fstream>
#include "grid.h"

extern Scalar myRed;
extern Scalar myGreen;
extern Scalar myBlue;
extern Scalar myPink;

vector<Point> processingGribovAlgorithm(
        vector<Point> contour,
        int dpEps,
        int gridStartX,
        int gridStartY,
        int gridIntervalX,
        int gridIntervalY,
        int prevPointsCount
        ) {
    vector<Point> dpContour;
    approxPolyDP(contour, dpContour, dpEps, true);

//    Mat gridImg = Mat::zeros(img.size(), CV_8UC3);
//    Mat rotatedContourImg = Mat::zeros(img.size(), CV_8UC3);
//
//    drawLines(rotatedContourImg, dpContour, myGreen);


    double rotationAngle = getRotationAngleInDeg(dpContour);
    Point initCentroid = getCentroidPoint(dpContour);
    vector<Point> rotatedContour = rotateContour(dpContour, rotationAngle);

    // drawLines(rotatedContourImg, rotatedContour, myRed);

    map<Point, vector<Point>, comparePoints> auxilaryPoints;

    for (const Point& rotatedPoint: rotatedContour) {
        Point nearestGridPoint = getNearestGridPoint(rotatedPoint, gridStartX, gridStartY, gridIntervalX, gridIntervalY);
        vector<Point> neighborPoints = getAuxilaryGridPoints(nearestGridPoint, gridIntervalX, gridIntervalY);
        auxilaryPoints[rotatedPoint] = neighborPoints;
        // drawPoints(rotatedContourImg, neighborPoints, myBlue);
    }

    // drawPoints(rotatedContourImg, rotatedContour, Scalar(0, 255, 255));

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

    //  double angleTolerance = 3;

    for (int pointNum = 1; pointNum < n; pointNum++) {
        // int pointNumMod = pointNum % n; //
        Point currentPoint = rotatedContour[pointNum];
        // Point currentPoint = rotatedContour[pointNumMod];
        tempSC.clear();
        tempDA.clear();
        tempBPP.clear();
        for (int auxNum = 0; auxNum < auxCount; auxNum++) {
            double minArea = INFINITY;
            int newSC = 0;
            pair<int, int> bestPoint;

            double minAreaNotStandard = INFINITY;
            int newSCNotStandard = 0;
            pair<int, int> bestPointNotStandard;
            double minAngleDiff = 45;

            Point currentAuxPoint = auxilaryPoints[currentPoint][auxNum];
            for (int prevPointNum = pointNum - 1; prevPointNum >= 0 && prevPointNum > pointNum - prevPointsCount; prevPointNum--) {
                Point prevPoint = rotatedContour[prevPointNum];
                for (int prevAuxNum = 0; prevAuxNum < auxCount; prevAuxNum++) {
                    Point prevAuxPoint = auxilaryPoints[prevPoint][prevAuxNum];
                    if (prevPointNum == 0 && pointNum != 1) {
                        continue;
                    }

                    bool angleIsStandard = true;
                    int angleMod = 0;
                    if (prevPointNum != 0) {
                        Point prevPrevPoint = getPrevPoint(prevPointNum, prevAuxNum, rotatedContour, bpp, auxilaryPoints);
                        double angle = getAngleBetweenSegments(prevPrevPoint, prevAuxPoint, currentAuxPoint);
                        double tempAngle = angle;
                        double normedAngle = angle - ((int)(angle / 45) * 45);
                        if (normedAngle < 45 && normedAngle > 22) {
                            tempAngle = 45 - normedAngle;
                        }
                        angleMod = (int)tempAngle % 45;
                        angleIsStandard = angleMod == 0;
                        if (!angleIsStandard && angleMod > minAngleDiff) {
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
                    double currentArea = getContourArea(newContour);
                    double allArea = currentArea + da[prevPointNum][prevAuxNum];
                    if (!angleIsStandard) {
                        if (angleMod == minAngleDiff) {
                            if (allArea < minAreaNotStandard) {
                                minAreaNotStandard = allArea;
                                bestPointNotStandard = make_pair(prevPointNum, prevAuxNum);
                                newSCNotStandard = sc[prevPointNum][prevAuxNum] + 1;
                            }
                        }
                        if (angleMod < minAngleDiff) {
                            minAngleDiff = angleMod;
                            minAreaNotStandard = allArea;
                            bestPointNotStandard = make_pair(prevPointNum, prevAuxNum);
                            newSCNotStandard = sc[prevPointNum][prevAuxNum] + 1;
                        }
                    } else {
                        if (allArea < minArea) {
                            minArea = allArea;
                            bestPoint = make_pair(prevPointNum, prevAuxNum);
                            newSC = sc[prevPointNum][prevAuxNum] + 1;
                        }
                    }
                }
            }
            if (newSC == 0) {
                tempSC.push_back(newSCNotStandard);
                tempDA.push_back(minAreaNotStandard);
                tempBPP.push_back(bestPointNotStandard);
            } else {
                tempSC.push_back(newSC);
                tempDA.push_back(minArea);
                tempBPP.push_back(bestPoint);
            }
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

    vector<Point> rightRotatedContour = rotateContourWithCentroid(rightContour, -rotationAngle, initCentroid);

    return rightRotatedContour;
}

/*!
 * Алгоритм Грибова
 * @param img - изображение (вообще-то не используется)
 * @param contour - контур
 * @param dpEps - аргумент для Дугласа-Пекера
 * @param gridStartX - координата x начала сетки
 * @param gridStartY - координата y начала сетки
 * @param gridIntervalX - расстояние между точками по x
 * @param gridIntervalY - расстояние между точками по y
 * @param prevPointsCount - количество предыдущих точек
 * @return векторизованные контуры
 */
vector<Point> processingGribovAlgorithm2(
        Mat img,
        vector<Point> contour,
        int dpEps,
        int gridStartX,
        int gridStartY,
        int gridIntervalX,
        int gridIntervalY,
        int prevPointsCount
) {
    vector<Point> dpContour;
    approxPolyDP(contour, dpContour, dpEps, true);

    // угол поворота
    double rotationAngle = getRotationAngleInDeg(dpContour);
    // центроид
    Point initCentroid = getCentroidPoint(dpContour);
    // повернутый контур
    vector<Point> rotatedContour = rotateContour(dpContour, rotationAngle);

    // словарь вспомогательных точек для точек контура
    map<Point, vector<Point>, comparePoints> auxilaryPoints;

    // находим для всех точек вспомогательные точки
    for (const Point& rotatedPoint: rotatedContour) {
        Point nearestGridPoint = getNearestGridPoint(rotatedPoint, gridStartX, gridStartY, gridIntervalX, gridIntervalY);
        vector<Point> neighborPoints = getAuxilaryGridPoints(nearestGridPoint, gridIntervalX, gridIntervalY);
        auxilaryPoints[rotatedPoint] = neighborPoints;
    }

    // segments count - количество сегментов
    vector<vector<int>> sc;
    // deviation area - площадь отклонения контуров
    vector<vector<int>> da;
    // pair int-int - номер предыдущей точки контура и номер ее вспомогательной точки
    vector<vector<pair<int, int>>> bpp;

    int n = rotatedContour.size();
    // количество вспомогательных точек для каждой точки контура
    int auxCount = 9;

    // находим текущие sc, da, bpp, которые добавятся в большой список
    vector<int> tempSC;
    vector<int> tempDA;
    vector<pair<int, int>> tempBPP;
    // инициализируем первые значения нулями
    for (int i = 0; i < auxCount; i++) {
        tempSC.push_back(0);
        tempDA.push_back(0);
        tempBPP.push_back(make_pair(0,0));
    }
    sc.push_back(tempSC);
    da.push_back(tempDA);
    bpp.push_back(tempBPP);

    // проходимся по всем точкам контура, начиная со первой (отсчет с нуля)
    for (int pointNum = 1; pointNum < n; pointNum++) {
        Point currentPoint = rotatedContour[pointNum];
        tempSC.clear();
        tempDA.clear();
        tempBPP.clear();
        // проходимся по всем вспомогательным точкам текущей точки контура
        for (int auxNum = 0; auxNum < auxCount; auxNum++) {
            // переменные для нахождения лучшей точки с минимальной площадью
            double minArea = INFINITY;
            int newSC = 0;
            pair<int, int> bestPoint;

            // переменные для нахождения лучшей точки с минимальной площадью, у которой угол, не кратен 45 градусам
            double minAreaNotStandard = INFINITY;
            int newSCNotStandard = 0;
            pair<int, int> bestPointNotStandard;
            double minAngleDiff = 45;

            Point currentAuxPoint = auxilaryPoints[currentPoint][auxNum];
            // проходимся по предыдущим точкам контура
            for (int prevPointNum = pointNum - 1; prevPointNum >= 0 && prevPointNum > pointNum - prevPointsCount; prevPointNum--) {
                Point prevPoint = rotatedContour[prevPointNum];
                // проходимся по вспомогательным точкам текущей предыдущей точки контура
                for (int prevAuxNum = 0; prevAuxNum < auxCount; prevAuxNum++) {
                    Point prevAuxPoint = auxilaryPoints[prevPoint][prevAuxNum];
                    if (prevPointNum == 0 && pointNum != 1) {
                        continue;
                    }

                    bool angleIsStandard = true;
                    int angleMod = 0;
                    if (prevPointNum != 0) {
                        // находим наилучшую предыдущую точку для текущей предыдущей
                        Point prevPrevPoint = getPrevPoint(prevPointNum, prevAuxNum, rotatedContour, bpp, auxilaryPoints);
                        // находим угол
                        double angle = getAngleBetweenSegments(prevPrevPoint, prevAuxPoint, currentAuxPoint);
                        double tempAngle = angle;
                        double normedAngle = angle - ((int)(angle / 45) * 45);
                        if (normedAngle < 45 && normedAngle > 22) {
                            tempAngle = 45 - normedAngle;
                        }
                        angleMod = (int)tempAngle % 45;
                        angleIsStandard = angleMod == 0;
                        // если угол не стандартный и уже найденный нестандартный угол ближе к нормальному, чем текущий,
                        // то переходим к следующей точке
                        if (!angleIsStandard && angleMod > minAngleDiff) {
                            continue;
                        }
                    }

                    // строим контур, по которому будем искать площадь отклонения
                    vector<Point> newContour;
                    newContour.push_back(currentPoint);
                    for (int j = pointNum - 1; j > prevPointNum; j--){
                        Point newPoint = rotatedContour[j];
                        newContour.push_back(newPoint);
                    }
                    newContour.push_back(prevPoint);
                    newContour.push_back(prevAuxPoint);
                    newContour.push_back(currentAuxPoint);
                    double currentArea = getContourArea(newContour);
                    double allArea = currentArea + da[prevPointNum][prevAuxNum];
                    // если угол нестандартный, то
                    if (!angleIsStandard) {
                        // если отклонение нестандартного угла от стандартного равно текущему минимальному отклонению
                        if (angleMod == minAngleDiff) {
                            // если площадь отклонения меньше, то текущий вариант лучше
                            if (allArea < minAreaNotStandard) {
                                minAreaNotStandard = allArea;
                                bestPointNotStandard = make_pair(prevPointNum, prevAuxNum);
                                newSCNotStandard = sc[prevPointNum][prevAuxNum] + 1;
                            }
                        }
                        // если отклонение нестандартного угла от стандартного равно меньше минимального отклонения,
                        // то текущий вариант лучше
                        if (angleMod < minAngleDiff) {
                            minAngleDiff = angleMod;
                            minAreaNotStandard = allArea;
                            bestPointNotStandard = make_pair(prevPointNum, prevAuxNum);
                            newSCNotStandard = sc[prevPointNum][prevAuxNum] + 1;
                        }
                        // если угол стандартный
                    } else {
                        // если площадь отклонения меньше, выбираем текущий вариант
                        if (allArea < minArea) {
                            minArea = allArea;
                            bestPoint = make_pair(prevPointNum, prevAuxNum);
                            newSC = sc[prevPointNum][prevAuxNum] + 1;
                        }
                    }
                }
            }
            // записываем результаты для текущей вершины
            if (newSC == 0) {
                tempSC.push_back(newSCNotStandard);
                tempDA.push_back(minAreaNotStandard);
                tempBPP.push_back(bestPointNotStandard);
            } else {
                tempSC.push_back(newSC);
                tempDA.push_back(minArea);
                tempBPP.push_back(bestPoint);
            }
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

    int lastIndex = rotatedContour.size() - 1;

    int minSC = INFINITY;
    int almostMinSC = INFINITY;
    vector<int> minAuxIndex;
    vector<int> almostMinIndex;

    // находим лучше вспомогательные точки последней вершины по количеству сегментов
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

    // находим наилучшую вспомогательную точку последней вершины по площади отклонения
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

    // берем лучшую конечную точку
    int currentPointIndex = lastIndex;
    int currentAuxIndex = bestAuxIndex;

    // составляем новый контур с конца
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

    // разворачиваем контур обратно
    vector<Point> rightRotatedContour = rotateContourWithCentroid(rightContour, -rotationAngle, initCentroid);

    return rightRotatedContour;
}

/*!
 * находит ближайшую вспомогательную точку для исходной
 * @param point - исходная точка
 * @param startX - x-координата начала сетки
 * @param startY - y-координата начала сетки
 * @param intervalX - интервал по x между точками сетки
 * @param intervalY - интервал по y между точками сетки
 * @return ближайшая вспомогательная точка
 */
Point getNearestGridPoint(Point point, int startX, int startY, int intervalX, int intervalY) {
    int nearestX = getNearestCoord(point.x, startX, intervalX);
    int nearestY = getNearestCoord(point.y, startY, intervalY);
    return Point(nearestX, nearestY);
}

/*!
 * находит ближайшую координату сетки по некоторой оси
 * @param pointCoord - координата исходной точки
 * @param gridStart - начало сетки по этой оси
 * @param gridInterval - интервал по оси между точками сетки
 * @return ближайшее значение координаты
 */
int getNearestCoord(int pointCoord, int gridStart, int gridInterval) {
    int dist = (pointCoord - gridStart) % gridInterval;
    if (dist < gridInterval / 2.0) {
        return pointCoord - dist;
    }
    return pointCoord + (gridInterval - dist);
}

/*!
 * находит все ближайшие вспомогательные точки
 * @param centerPoint - центральная точка
 * @param intervalX - интервал между точками по x
 * @param intervalY - интервал между точками по y
 * @return массив вспомогательных точек
 */
vector<Point> getAuxilaryGridPoints(Point centerPoint, int intervalX, int intervalY) {
    vector<Point> neighborPoints;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            neighborPoints.push_back(Point(centerPoint.x + i * intervalX, centerPoint.y + j * intervalY));
        }
    }
    return neighborPoints;
}

/*!
 * находит угол между отрезками
 * @param point1 - точка начала первого отрезка
 * @param point2 - точка конца первого отрезка и начала второго отрезка
 * @param point3 - точка конца второго отрезка
 * @return угол между отрезками
 */
double getAngleBetweenSegments(Point point1, Point point2, Point point3) {
    Point q1 = Point(point2.x - point1.x, point2.y - point1.y);
    Point q2 = Point(point3.x - point2.x, point3.y - point2.y);
    double temp = (q1.x * q2.x + q1.y * q2.y) /
                  (sqrt(q1.x * q1.x+ q1.y * q1.y) * sqrt(q2.x * q2.x+ q2.y * q2.y));
    double angleInRad = acos(temp);
    return fromRadToDeg(angleInRad);
}

/*!
 * находит наилучшую предыдущую точку
 * @param pointNum - номер точки
 * @param auxNum - номер вспомогательной точки
 * @param contour - контур
 * @param bpp - best previous points
 * @param auxilaryPoints - всопомогательные точки
 * @return наилучшая предыдущая точка
 */
Point getPrevPoint(int pointNum, int auxNum, vector<Point> contour,
        vector<vector<pair<int, int>>> bpp, map<Point, vector<Point>, comparePoints> auxilaryPoints) {
    int prevPointNum = bpp[pointNum][auxNum].first;
    int prevAuxPointNum = bpp[pointNum][auxNum].second;
    Point prevPoint = auxilaryPoints[contour[prevPointNum]][prevAuxPointNum];
    return prevPoint;
}
