#include <iostream>
#include <stack>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "Contour.cpp"

using namespace cv;
using namespace std;

void sobelFunc();
void cannyFunc();
void houghFunc();
void first();
void convexHull();
void contours();
void rotatedMinAreaRect();
void projection();
vector<Point> douglasPeucker(vector<Point> polyline, double eps);
vector<Point> ramerDouglasPeuckerRecr(vector<Point> polyline, double eps);
double perpendicularLength(Point, Point, Point);
void test();

Scalar red = Scalar(0, 0, 255);
Scalar green = Scalar(0, 255, 0);
Scalar blue = Scalar(255, 0, 0);

int main() {
    // sobelFunc();
    // cannyFunc();
    // houghFunc();
    // first();
    // contours();
    // convexHull();
    // rotatedMinAreaRect();
    projection();
    // test();
}

void test() {
    vector<Point> points = {Point(0, 0)};
    Contour contour = Contour(points);
    contour.print();
}

tuple<vector<Point>, bool> doubleContourToSimple(vector<Point> points) {
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

void printPoints(vector<Point> points) {
    for(Point point: points) {
        cout << point.x << ";" << point.y << endl;
    }
}


void showImg(Mat img, String imgName) {
    imshow(imgName, img);
    waitKey(0);
}

void drawLines(Mat img, vector<Point> points, Scalar color, bool joinEnds = true, bool isShowImg = false, String imgName = "") {
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

    for (int eps = 1; eps <= 4; eps++) {
        for (size_t i = 0; i < contours.size(); i++) {
            vector<Point> contour = contours[i];
            tuple<vector<Point>, bool> simpleResult = doubleContourToSimple(contour);
            vector<Point> simple = get<0>(simpleResult);
            bool isSimpled = get<1>(simpleResult);

            vector<Point> result = ramerDouglasPeuckerRecr(simple, eps);
            result = deleteRepeatedNeighborPoints(result);

            int pointsLength = result.size();
            drawLines(simplContoursImg, result, green, !isSimpled);
            drawPoints(simplContoursImg, result, blue);

            cout << i << endl;
            cout << "orig " << contour.size() << " simple " << simple.size() << " dugl " << pointsLength << endl;

        }
        imshow("Building contours", simplContoursImg);
        imwrite(format("../ramerDouglasPeucker%d.jpg", eps), simplContoursImg);

        waitKey(0);
        simplContoursImg = Mat::zeros(img.size(), CV_8UC3);
    }
    waitKey(0);
}

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

double perpendicularLength(Point first, Point second, Point zero) {
    int triangleArea = abs((first.y - second.y) * zero.x - (first.x - second.x) * zero.y + (first.x * second.y) - (second.x * first.y));
    double firstSecondLength = sqrt(pow(second.y - first.y, 2) + pow(second.x - first.x, 2));
    return (double)triangleArea / firstSecondLength;
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

void first() {
    RNG rng(12345);
    Mat img = imread("../satimg.jpg");
    Mat grayImg;
    cvtColor(img, grayImg, COLOR_BGR2GRAY);
    Mat cannyOutput;
    Canny(grayImg, cannyOutput, 100, 255);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(cannyOutput, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    Mat drawing = Mat::zeros(cannyOutput.size(), CV_8UC3);
    for (size_t i = 0; i < contours.size(); i++) {
        Scalar color = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
        drawContours(drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0);
    }
    imshow("Contours", drawing);
    waitKey(0);
}

void sobelFunc() {
    Mat img = imread("../satimg.jpg");
    Mat sobelX;
    Mat sobelY;
    Mat sobel;
    // Scharr(img, sobelX, CV_16S, 1, 0, 3);
    // Scharr(img, sobelY, CV_16S, 0, 1, 3);
    Sobel(img, sobelX, CV_16S, 1, 0);
    Sobel(img, sobelY, CV_16S, 0, 1);
    // Mat norm , dir;
    // cartToPolar(sobelX, sobelY, norm, dir);
    sobel = abs(sobelX) + abs(sobelY);
    imshow("X", sobelX);
    imshow("Y", sobelY);
    imshow("Sobel", sobel);
    waitKey(0);
}

void cannyFunc() {
    Mat img = imread("../satimg.jpg");
    Mat contours;
    Canny(img, contours, 125, 350);
    imshow("Contours", contours);
    imshow("Original", img);
    waitKey(0);
}

void houghFunc() {
    Mat img = imread("../satimg.jpg");
    Mat contours, contours2;
    Canny(img, contours, 125, 350);
    cvtColor(contours, contours2, COLOR_GRAY2BGR);

    vector<Vec2f> lines;
    HoughLines(contours, lines, 1, CV_PI/180, 100, 0, 0 );

    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        line(contours2, pt1, pt2, Scalar(0,0,255), 1);
    }

    imshow("Contours", contours2);
    waitKey(0);
}
