//
// Created by Dariya Petrova on 11.05.2020.
//

#include <iostream>
#include <fstream>
#include "contourUtils.h"

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

// двойной контур, в котором дважды повторяются точки и у которого начало где-то не в начале, переводит в одиночный
// актуально для незамкнутых контуров
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