//
// Created by Dariya Petrova on 19.05.2020.
//

#pragma once

#include <CGAL/Point_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Simple_cartesian.h>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef CGAL::Polygon_2<Kernel> Polygon_2;
typedef Kernel::Point_2 Point_2;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "drawing.h"

void intersect(vector<Point> contour1, vector<Point> contour2);
