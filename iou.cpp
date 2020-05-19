//
// Created by Dariya Petrova on 19.05.2020.
//

#include "iou.h"

void intersect(vector<Point> contour1, vector<Point> contour2) {
    Point_2 point(1, 2);
    Point_2 points[] = { point };
    Polygon_2 polygon(points, points + 1);
    Polygon_2 result;
    intersection(polygon, polygon, result);
}
