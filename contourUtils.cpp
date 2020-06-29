//
// Created by Dariya Petrova on 11.05.2020.
//

#include <iostream>
#include <fstream>
#include "contourUtils.h"

/*!
 * получить площадь внутри контура
 * @param contour - контур
 * @return площадь внутри контура
 */
int getContourArea(vector<Point> contour) {
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
            double test = pointPolygonTest(contour, Point(x, y), false);
            if (test == 1 || test == 0) {
                cntArea += 1;
            }
        }
    }

    return cntArea;
}

/*!
 * проверка, является ли контур замкнутым
 * @param contour - исходный контур
 * @return true, если замкнутый, false иначе
 */
bool isClosedContour(vector<Point> contour) {
    for (int i = 0; i < contour.size(); i++) {
        if (i - 1 != 0 && i + 1 != contour.size()) {
            if (contour[i - 1] == contour[i + 1]) {
                return false;
            }
        }
    }
    return true;
}

/*!
 * переводит контур, у которого дважды повторятся точки в нормальный. актуально для незамкнутых контуров.
 * @param contour - исходный контур
 * @return если был двойным, то возвращает одиночный, иначе возвращает исходный
 */
vector<Point> doubleContourToSingle(vector<Point> contour) {
    vector<Point> result;
    bool hasStart = false;
    for (int i = 0; i < contour.size(); i++) {
        if (!hasStart &&
            i - 1 != 0 &&
            i + 1 != contour.size() &&
            contour[i - 1] == contour[i + 1]
        ) {
            result.push_back(contour[i]);
            hasStart = true;
            continue;
        }
        if (hasStart) {
            result.push_back(contour[i]);
        }
        if (i - 1 != 0 &&
            i + 1 != contour.size() &&
            contour[i - 1] == contour[i + 1]
        ) {
            break;
        }
    }
    if (result.empty()) {
        return contour;
    }

    return result;
}

// может ли контур быть описан прямоугольником с ограничением на максимальную разность между площадями
/*!
 * проверка, может ли контур быть описан прямоугольником
 * @param contour - контур
 * @param maxDiff - максимальная разность между площадями контура и прямоугольника
 * @return true, если может описать, false - иначе
 */
bool canBeDescribedByRectOld(vector<Point> contour, int maxDiff) {
    RotatedRect rect = minAreaRect(contour);
    double rectArea = rect.size.area();
    double contourArea = getContourArea(contour);
    double diff = abs(rectArea - contourArea);
    return diff < maxDiff;
}

bool canBeDescribedByRect(vector<Point> contour) {
    RotatedRect rect = minAreaRect(contour);
    double rectArea = rect.size.area();
    double contourArea = getContourArea(contour);
    double ratio = abs(rectArea - contourArea) / rectArea;
    return ratio < 0.2;
}

/*!
 * нахождение контура прямоугольника, описывающего исходный контур
 * @param contour - исходный контур
 * @return
 */
vector<Point> processingMinAreaRect(vector<Point> contour) {
    RotatedRect rect = minAreaRect(contour);
    Point2f rectPoints[4];
    rect.points(rectPoints);
    vector<Point> result;
    for (Point point: rectPoints) {
        result.push_back(point);
    }
    return result;
}

// проходит по точкам и удаляет дублеров-соседей
vector<Point> deleteRepeatedNeighborPoints(vector<Point> points) {
    vector<Point> result;
    result.push_back(points[0]);
    for (int i = 1; i < points.size(); i++) {
        if (points[i] != points[i-1]) {
            result.push_back(points[i]);
        }
    }
    return result;
}