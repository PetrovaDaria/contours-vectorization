//
// Created by Dariya Petrova on 02.04.2020.
//
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

class Contour {

private:
    vector<Point> vertices;

public:
    explicit Contour(vector<Point> &vertices): vertices(vertices) {

    }

    void print() {
        for (const Point& point: vertices) {
            cout << point.x << " " << point.y << endl;
        }
    }
};

