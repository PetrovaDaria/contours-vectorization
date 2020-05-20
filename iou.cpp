//
// Created by Dariya Petrova on 19.05.2020.
//

#include "iou.h"

double intersect(vector<Point> contour1, vector<Point> contour2) {
    Polygon_2 P;
//    for (Point p: contour1) {
//        double x = p.x;
//        double y = p.y;
//        Point_2 newPoint = Point_2(x, y);
//        P.push_back(newPoint);
//    }
    P.push_back (Point_2 (0, 0));
    P.push_back (Point_2 (5, 0));
    P.push_back (Point_2 (3.5, 1.5));
    P.push_back (Point_2 (2.5, 0.5));
    P.push_back (Point_2 (1.5, 1.5));
     // std::cout << "P = "; print_polygon (P);
    Polygon_2 Q;
//    for (Point p: contour2) {
//        double x = p.x;
//        double y = p.y;
//        Q.push_back(Point_2(x, y));
//    }
    Q.push_back (Point_2 (0, 2));
    Q.push_back (Point_2 (1.5, 0.5));
    Q.push_back (Point_2 (2.5, 1.5));
    Q.push_back (Point_2 (3.5, 0.5));
    Q.push_back (Point_2 (5, 2));
    // std::cout << "Q = "; print_polygon (Q);
    // Compute the union of P and Q.
    Polygon_with_holes_2 unionR;
    if (CGAL::join (P, Q, unionR)) {
        std::cout << "The union: ";
        // print_polygon_with_holes (unionR);
    } else
        std::cout << "P and Q are disjoint and their union is trivial."
                  << std::endl;
    std::cout << std::endl;
    // Compute the intersection of P and Q.
    Pwh_list_2                  intR;
    Pwh_list_2::const_iterator  it;
    CGAL::intersection (P, Q, std::back_inserter(intR));
    std::cout << "The intersection:" << std::endl;
    for (it = intR.begin(); it != intR.end(); ++it) {
        std::cout << "--> ";
        // print_polygon_with_holes (*it);
    }

//    Point_2 points1[] = {
//            Point_2(3, 2),
//            Point_2(5, 2),
//            Point_2(5, 4),
//            Point_2(7, 4),
//            Point_2(7, 8),
//            Point_2(6, 8),
//            Point_2(6, 10),
//            Point_2(2, 10),
//            Point_2(2, 7),
//            Point_2(3, 7),
//            Point_2(3, 5),
//            Point_2(1, 5),
//            Point_2(1, 3),
//            Point_2(3, 3),
//    };
//    Point_2 points2[] = {
//            Point_2(7, 1),
//            Point_2(11, 1),
//            Point_2(11, 8),
//            Point_2(8, 8),
//            Point_2(8, 11),
//            Point_2(4, 11),
//            Point_2(4, 7),
//            Point_2(10, 7),
//            Point_2(10, 5),
//            Point_2(4, 5),
//            Point_2(4, 3),
//            Point_2(7, 3),
//    };
//    Polygon_2 polygon1;
//    for (Point point: contour1) {
//        polygon1.push_back(Point_2(point.x, point.y));
//    }
//    Polygon_2 polygon2;
//    for (Point point: contour2) {
//        polygon2.push_back(Point_2(point.x, point.y));
//    }
//    Pwh_list_2 result;
//    intersection(polygon1, polygon2, back_inserter(result));
//    double totalArea = 0;
//    for (Polygon_with_holes_2 poly: result) {
//        poly.outer_boundary().area();
//    }
//    typedef Pwh_list_2 ::iterator LIT;
//    for(LIT lit = result.begin(); lit!=result.end(); lit++){
//        totalArea += lit.outer_boundary().area();
//    }
    //return totalArea;
    return 0;
}
