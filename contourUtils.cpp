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

// двойной контур, в котором дважды повторяются точки и у которого начало где-то не в начале, переводит в одиночный
// актуально для незамкнутых контуров
tuple<vector<Point>, bool> doubleContourToSingle(vector<Point> points) {
    vector<Point> result;
    bool hasStart = false;
    for (int i = 0; i < points.size(); i++) {
        if (!hasStart &&
            i - 1 != 0 && i + 1 != points.size() &&
            points[i-1] == points[i+1]) {
            result.push_back(points[i]);
            hasStart = true;
            continue;
        }
        if (hasStart) {
            result.push_back(points[i]);
        }
        if (i - 1 != 0 && i + 1 != points.size() && points[i-1] == points[i+1]) {
            break;
        }
    }
    if (result.size() == 0) {
        return make_tuple(points, false);
    }

    return make_tuple(result, true);
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