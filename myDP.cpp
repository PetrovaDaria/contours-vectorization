//
// Created by Dariya Petrova on 11.05.2020.
//
#include "myDP.h"
using namespace std;
using namespace cv;

// высчитывает длину перпенидкуляра от точки zero до отрезка first-second через формулу площади треугольника
double perpendicularLength(Point first, Point second, Point zero) {
    int triangleArea = abs((first.y - second.y) * zero.x - (first.x - second.x) * zero.y + (first.x * second.y) - (second.x * first.y));
    double firstSecondLength = sqrt(pow(second.y - first.y, 2) + pow(second.x - first.x, 2));
    return (double)triangleArea / firstSecondLength;
}

// рекурсивный Дуглас-Пекер
vector<Point> ramerDouglasPeuckerRecr(vector<Point> polyline, double eps) {
    Point first = polyline[0];
    Point last = polyline[polyline.size() - 1];
    double maxDistance = 0;
    int index = 0;
    for (int i = 1; i < polyline.size() - 1; i++) {
        double perpendicular = perpendicularLength(first, last, polyline[i]);
        if (perpendicular > maxDistance) {
            maxDistance = perpendicular;
            index = i;
        }
    }
    if (maxDistance >= eps) {
        vector<Point> left(&polyline[0], &polyline[index+1]);
        vector<Point> right(&polyline[index], &polyline[polyline.size()]);
        vector<Point> leftPolyLine = ramerDouglasPeuckerRecr(left, eps);
        vector<Point> rightPolyline = ramerDouglasPeuckerRecr(right, eps);
        leftPolyLine.insert(leftPolyLine.end(), rightPolyline.begin(), rightPolyline.end());
        return leftPolyLine;
    } else {
        vector<Point> result;
        result.push_back(polyline[0]);
        result.push_back(polyline[polyline.size() - 1]);
        return result;
    }
}

// как-то неправильно работает
vector<Point> douglasPeucker(vector<Point> polyline, double eps) {
    stack<Range> stack;
    bool points[polyline.size()];
    stack.push(Range( 0, polyline.size() - 1 ));
    while (!stack.empty()) {
        Range current = stack.top();
        int startIndex = current.start;
        int endIndex = current.end;
        stack.pop();
        double maxDist = 0;
        int index = startIndex;
        for (int i = startIndex + 1; i < endIndex; i++) {
            if (points[i]) {
                double distance = perpendicularLength(polyline[startIndex], polyline[endIndex], polyline[i]);
                if (distance > maxDist) {
                    index = i;
                    maxDist = distance;
                }
            }
        }
        if (maxDist >= eps) {
            stack.push(Range(startIndex, index));
            stack.push(Range(index, endIndex));
        } else {
            for (int i = startIndex + 1; i < endIndex; i++) {
                points[i] = false;
            }
        }
    }

    vector<Point> result;
    for (int i = 0; i < polyline.size(); i++) {
        if (points[i]) {
            result.push_back(polyline[i]);
        }
    }
    return result;
}
