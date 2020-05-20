//
// Created by Dariya Petrova on 19.05.2020.
//

#pragma once

#include <CGAL/Point_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

//typedef CGAL::Simple_cartesian<double> SKernel;
//// typedef CGAL::Epeck SKernel;
//typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
//typedef CGAL::Polygon_2<Kernel> Polygon_2;
//typedef Kernel::Point_2 Point_2;
//typedef CGAL::Polygon_with_holes_2<Kernel> Polygon_with_holes_2;
//typedef std::list<Polygon_with_holes_2> Pwh_list_2;

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef Kernel::Point_2                                   Point_2;
typedef CGAL::Polygon_2<Kernel>                           Polygon_2;
typedef CGAL::Polygon_with_holes_2<Kernel>                Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>                   Pwh_list_2;

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "drawing.h"

double intersect(vector<Point> contour1, vector<Point> contour2);
