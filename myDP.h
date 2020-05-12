//
// Created by Dariya Petrova on 11.05.2020.
//

#pragma once

double perpendicularLength(Point first, Point second, Point zero);
vector<Point> ramerDouglasPeuckerRecr(vector<Point> polyline, double eps);
vector<Point> douglasPeucker(vector<Point> polyline, double eps);
