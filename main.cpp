#include <iostream>
#include <stack>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "main.h"
using namespace cv;
using namespace std;

// contour utils
tuple<vector<Point>, bool> doubleContourToSingle(vector<Point> points);
vector<Point> deleteRepeatedNeighborPoints(vector<Point> points);

// drawing
Scalar red(Scalar(0, 0, 255));
Scalar green(Scalar(0, 255, 0));
Scalar blue(Scalar(255, 0, 0));
void showImg(Mat img, String imgName);
void drawLines(Mat img, vector<Point> points, Scalar color, bool joinEnds, bool isShowImg,
               String imgName);
void drawPoints(Mat img, vector<Point> points, Scalar color);
void printPoints(vector<Point> points);

// grid
void gribovAlgorithm();

// my dp
double perpendicularLength(Point first, Point second, Point zero);
vector<Point> ramerDouglasPeuckerRecr(vector<Point> polyline, double eps);
vector<Point> douglasPeucker(vector<Point> polyline, double eps);


int main() {
    gribovAlgorithm();
    // rotatedMinAreaRect();
    // !!! projection();
    // integratedDP();
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

void showImg(Mat img, String imgName) {
    imshow(imgName, img);
    waitKey(0);
}

void drawLines(Mat img, vector<Point> points, Scalar color, bool joinEnds = true, bool isShowImg = false,
               String imgName = "") {
    int size = points.size();
    int end = size;
    if (!joinEnds) {
        end -= 1;
    }
    for (int j = 0; j < end; j++) {
        line(img, points[j], points[(j + 1) % size], color);
        if (isShowImg) {
            showImg(img, imgName);
        }
    }
}

void drawPoints(Mat img, vector<Point> points, Scalar color) {
    int size = points.size();
    for (int j = 0; j < size; j++) {
        line(img, points[j], points[j], color);
    }
}

void printPoints(vector<Point> points) {
    for(Point point: points) {
        cout << point.x << ";" << point.y << endl;
    }
}

void gribovAlgorithm() {
    Mat img = imread("../oneBuilding.png");
    cout << img.size() << endl;

    Mat contoursImg;
    Canny(img, contoursImg, 100, 255);
    imshow("Contours", contoursImg);
    imwrite("../oneBuildingGrid.jpg", contoursImg);

    vector<vector<Point>> contours;
    findContours(contoursImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    cout << contours[0].size() << endl;

    vector<Point> dpContour;
    // dpContour = myDp::ramerDouglasPeuckerRecr(contours[0], 2);
    approxPolyDP(contours[0], dpContour, 2, true);
    printPoints(dpContour);

    cout << dpContour.size() << endl;

    Mat gridImg = Mat::zeros(img.size(), CV_8UC3);
    drawLines(gridImg, contours[0], blue);
    showImg(gridImg, "contour");
    gridImg = Mat::zeros(img.size(), CV_8UC3);
    drawLines(gridImg, dpContour, green);
    showImg(gridImg, "dp contour");
}

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

double getLength(Point start, Point end) {
    return sqrt(pow((end.x - start.x), 2) + pow((end.y - start.y), 2));
}

Point getIntersectionOfLineAndPointPerpendicular(Point lineStart, Point lineEnd, Point point) {
    double a0 = point.x - lineStart.x;
    double a1 = lineEnd.x - lineStart.x;
    double a2 = point.y - lineStart.y;
    double a3 = lineEnd.y - lineStart.y;
    double a4 = a0 * a1 + a2 * a3;
    double a5 = a1 * a1;
    double a6 = a2 * a2;
    double a7 = sqrt(a5 + a6);
    double x = lineStart.x + (a4/a7)*(a1/a7);
    double y = lineStart.y + (a4/a7)*(a3/a7);
    return Point(x, y);
}

vector<double> getSidesLengths(vector<Point> points, bool joinEnds = false) {
    vector<double> lengths;
    for (int i = 1; i < points.size(); i++) {
        Point start = points[i-1];
        Point end = points[i];
        double length = getLength(start, end);
        lengths.push_back(length);
    }
    if (joinEnds) {
        Point start = points[points.size() - 1];
        Point end = points[0];
        double length = getLength(start, end);
        lengths.push_back(length);
    }
    return lengths;
}

void integratedDP() {
    Mat img = imread("../satimg.jpg");

    Mat contoursImg;
    Canny(img, contoursImg, 100, 255);

    vector<vector<Point>> contours;
    findContours(contoursImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> dpContours;
    for (int i = 0; i < contours.size(); i++) {
        vector<Point> contour = contours[i];
        vector<Point> dpContour;
        approxPolyDP(contour, dpContour, 2, false);
        dpContours.push_back(dpContour);
        cout << i << "  " << contour.size() << "  " << dpContour.size() << endl;
    }

    Mat integratedDpContoursImg = Mat::zeros(img.size(), CV_8UC3);
    for (int i = 0; i < dpContours.size(); i++) {
        drawLines(integratedDpContoursImg, dpContours[i], green);
        drawPoints(integratedDpContoursImg, dpContours[i], red);
    }
    imshow("DP", integratedDpContoursImg);
    waitKey(0);
}

void projection() {
    Mat img = imread("../satimg.jpg");
    cout << img.size() << endl;

    Mat contoursImg;
    Canny(img, contoursImg, 100, 255);
    imshow("Contours", contoursImg);
    imwrite("../contours.jpg", contoursImg);

    vector<vector<Point>> contours;
    findContours(contoursImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Mat simplContoursImg = Mat::zeros(img.size(), CV_8UC3);
    Mat integratedDpContoursImg = Mat::zeros(img.size(), CV_8UC3);

    vector<Point> fifteenth = contours[15];
    fifteenth = get<0>(doubleContourToSingle(fifteenth));
    tuple<vector<Point>, bool> fifteenthResult = doubleContourToSingle(fifteenth);
    fifteenth = get<0>(fifteenthResult);
    bool isSingled = get<1>(fifteenthResult);
    isSingled = true;
    fifteenth = ramerDouglasPeuckerRecr(fifteenth, 2);
    fifteenth = deleteRepeatedNeighborPoints(fifteenth);
    printPoints(fifteenth);
    drawLines(simplContoursImg, fifteenth, blue, !isSingled);
    drawPoints(simplContoursImg, fifteenth, red);
    imshow("Name", simplContoursImg);
    waitKey(0);
    simplContoursImg = Mat::zeros(img.size(), CV_8UC3);
    vector<double> lengths = getSidesLengths(fifteenth, isSingled);
    for (double length: lengths) {
        cout << length << endl;
    }
    int maxElementIndex = max_element(lengths.begin(), lengths.end()) - lengths.begin();
    cout << maxElementIndex << endl;
    for (int i = 1; i < fifteenth.size(); i++) {
        int currentIndex = (maxElementIndex + i + 1) % fifteenth.size();
        if (isSingled & (currentIndex == 0 || currentIndex == 1)) {
            continue;
        } else {
            Point lineStart = fifteenth[(maxElementIndex + i - 1) % fifteenth.size()];
            Point lineEnd = fifteenth[(maxElementIndex + i) % fifteenth.size()];
            Point point = fifteenth[(maxElementIndex + i + 1) % fifteenth.size()];
            Point newPoint = getIntersectionOfLineAndPointPerpendicular(lineStart, lineEnd, point);
            fifteenth[(maxElementIndex + i) % fifteenth.size()] = newPoint;
        }
    }
    drawLines(simplContoursImg, fifteenth, blue, !isSingled);
    drawPoints(simplContoursImg, fifteenth, red);
    imshow("Name", simplContoursImg);
    waitKey(0);

    for (int eps = 1; eps <= 4; eps++) {
        int equals = 0;
        int myBigger = 0;
        int mySmaller = 0;
        int smallerDiff = 0;
        int biggerDiff = 0;
        for (size_t i = 0; i < contours.size(); i++) {
            vector<Point> contour = contours[i];
            tuple<vector<Point>, bool> singleResult = doubleContourToSingle(contour);
            vector<Point> singled = get<0>(singleResult);
            bool isSingled = get<1>(singleResult);

            vector<Point> result = ramerDouglasPeuckerRecr(singled, eps);
            result = deleteRepeatedNeighborPoints(result);

            int pointsLength = result.size();
            drawLines(simplContoursImg, result, green, !isSingled);
            drawPoints(simplContoursImg, result, blue);

            vector<Point> dpContour;
            approxPolyDP(contour, dpContour, eps, true);

            drawLines(integratedDpContoursImg, dpContour, green );
            drawPoints(integratedDpContoursImg, dpContour, blue);

            if (dpContour.size() == pointsLength) {
                equals += 1;
            } else if (dpContour.size() > pointsLength) {
                mySmaller += 1;
                smallerDiff += dpContour.size() - pointsLength;
            } else if (dpContour.size() < pointsLength) {
                myBigger += 1;
                biggerDiff += pointsLength - dpContour.size();
            }

            cout << i << "  " << contour.size() << "  " << pointsLength << endl;
            // cout << i << endl;
            // cout << "orig " << contour.size() << " singled " << singled.size() << " dugl " << pointsLength << endl;

        }
        cout << "my smaller: " << mySmaller <<
        " equals: " << equals <<
        " my bigger: " << myBigger <<
        " smaller diff " << smallerDiff <<
        " bigger diff " << biggerDiff << endl;
        imshow("Building contours", simplContoursImg);
        imwrite(format("../ramerDouglasPeucker%d.jpg", eps), simplContoursImg);

        imshow("DP", integratedDpContoursImg);
        imwrite(format("../integreatedDP%d.jpg", eps), integratedDpContoursImg);

        waitKey(0);
        simplContoursImg = Mat::zeros(img.size(), CV_8UC3);
        integratedDpContoursImg = Mat::zeros(img.size(), CV_8UC3);
    }
    waitKey(0);
}

void rotatedMinAreaRect() {
    Mat img = imread("../satimg.jpg");
    Mat grayImg, cannyOutput;
    cvtColor(img, grayImg, COLOR_BGR2GRAY);
    Canny(grayImg, cannyOutput, 100, 255);
    vector<vector<Point>> contours;
    findContours(cannyOutput, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    vector<vector<Point>> hull(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        convexHull(contours[i], hull[i]);
    }
    Mat drawing = Mat::zeros(cannyOutput.size(), CV_8UC3);
    Scalar contourColor = Scalar(0, 255, 0);
    Scalar hullColor = Scalar(255, 0, 0);
    Scalar rectColor = Scalar(0, 0, 255);
    cout << contours.size();
    for (size_t i = 0; i < contours.size(); i++) {
        vector<Point> contour = contours[i];
        RotatedRect boundRect = minAreaRect(contour);
        cout << "Contour size " << contour.size() << endl;
        cout << "Contour area " << contourArea(contour) << endl;
        cout << "Rect area " << boundRect.size.area() << endl;
        Point2f rectPoints[4];
        boundRect.points(rectPoints);
        drawContours(drawing, contours, (int)i, contourColor);
        char buffer[100] = {};
        sprintf(buffer, "%f %f", contourArea(contour), boundRect.size.area());
        for ( int j = 0; j < 4; j++ )
        {
            line( drawing, rectPoints[j], rectPoints[(j+1)%4], rectColor );
        }
        vector<Point> poly;
        approxPolyDP(contour, poly, 5, true);
        polylines(drawing, poly, true, Scalar(255, 0, 255), 1);
    }
    namedWindow("Min area rect", WINDOW_NORMAL);
    imshow("Min area rect", drawing);
    imwrite("../minAreaRect.jpg", drawing);
    waitKey(0);
}

void convexHull() {
    Mat img = imread("../satimg.jpg");
    Mat grayImg, cannyOutput;
    cvtColor(img, grayImg, COLOR_BGR2GRAY);
    Canny(grayImg, cannyOutput, 100, 255);
    vector<vector<Point>> contours;
    findContours(cannyOutput, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    vector<vector<Point>> hull(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        convexHull(contours[i], hull[i]);
    }
    Mat drawing = Mat::zeros(cannyOutput.size(), CV_8UC3);
    Scalar contourColor = Scalar(0, 0, 255);
    Scalar hullColor = Scalar(0, 255, 0);
    Scalar rectColor = Scalar(255, 0, 0);
    cout << contours.size();
    for (size_t i = 0; i < contours.size(); i++) {
        Rect boundRect = boundingRect(contours[i]);
        rectangle(drawing, boundRect.tl(), boundRect.br(), rectColor, 2);
        drawContours(drawing, contours, (int)i, contourColor);
        drawContours(drawing, hull, (int)i, hullColor);
    }
    imshow("Convex hull", drawing);
    waitKey(0);
}

void contours() {
    Mat img = imread("../satimg.jpg");
    imshow("Original", img);
    Mat closed, opened, grayImg, cannyOutput;
    Mat element5(5, 5, CV_8U, Scalar(1));
    morphologyEx(img, closed, MORPH_CLOSE, element5);
    imshow("Closed", closed);
    morphologyEx(closed, opened, MORPH_OPEN, element5);
    imshow("Opened", opened);
    cvtColor(closed, grayImg, COLOR_BGR2GRAY);
    Canny(grayImg, cannyOutput, 100, 255);
    vector<vector<Point>> contours;
    findContours(cannyOutput, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);
    vector<vector<Point>> hull(contours.size());
    for (size_t i = 0; i < contours.size(); i++) {
        convexHull(contours[i], hull[i]);
    }
    Mat drawing = Mat::zeros(cannyOutput.size(), CV_8UC3);
    Scalar contourColor = Scalar(0, 0, 255);
    Scalar hullColor = Scalar(0, 255, 0);
    Scalar rectColor = Scalar(255, 0, 0);
    cout << contours.size();
    for (size_t i = 0; i < contours.size(); i++) {
        Rect boundRect = boundingRect(contours[i]);
        rectangle(drawing, boundRect.tl(), boundRect.br(), rectColor, 2);
        drawContours(drawing, contours, (int)i, contourColor);
        drawContours(drawing, hull, (int)i, hullColor);
    }
    imshow("Contours", drawing);
    waitKey(0);
}
