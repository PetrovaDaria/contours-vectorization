#include <iostream>
#include <stack>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

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
vector<Point> douglasPeuckerRecr(vector<Point> polyline, double eps);
double perpendicularLength(Point, Point, Point);

int main() {
    // sobelFunc();
    // cannyFunc();
    // houghFunc();
    // first();
    // contours();
    // convexHull();
    // rotatedMinAreaRect();
    projection();
}

void projection() {
    Mat img = imread("../satimg.jpg");
    Mat cannyImg;
    Canny(img, cannyImg, 100, 255);
    imshow("Canny", cannyImg);

    vector<vector<Point>> cannyContours;
    findContours(cannyImg, cannyContours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    Mat cannyContoursImg = Mat::zeros(img.size(), CV_8UC3);

    double eps = 3;

    for (size_t i = 0; i < cannyContours.size(); i++) {
        // cout << cannyContours[i] << endl;
        drawContours(cannyContoursImg, cannyContours, (int)i, Scalar(255, 0, 0));

        vector<Point> experiment = cannyContours[i];
        // cout << experiment << endl;
        // cout << experiment.size() << endl;
        vector<Point> result = douglasPeuckerRecr(experiment, eps);
        // vector<Point> result = douglasPeucker(experiment, eps);
        // cout << result << endl;
        // cout << result2.size() << endl;
        cout << result.size() << endl;

        int pointsLength = result.size();
        for ( int j = 0; j < pointsLength; j++ )
        {
            line(cannyContoursImg, result[j], result[(j+1)%pointsLength], Scalar(0, 255, 0) );
        }
        for ( int j = 0; j < pointsLength; j++ )
        {
            line(cannyContoursImg, result[j], result[j], Scalar(0, 0, 255) );
        }

        cout << "orig " << experiment.size() << " dugl " << pointsLength << endl;
    }

    imshow("Canny contours", cannyContoursImg);
    imwrite("../minAreaRect.jpg", cannyContoursImg);

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

vector<Point> douglasPeuckerRecr(vector<Point> polyline, double eps) {
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
    // cout << "max dist " << maxDistance << endl;
    // cout << "index " << index << endl;
    if (maxDistance >= eps) {
        vector<Point> left(&polyline[0], &polyline[index]);
        vector<Point> right(&polyline[index], &polyline[polyline.size() - 1]);
        // cout << "Left " << left << endl;
        // cout << "Right " << right << endl;
        vector<Point> leftPolyLine = douglasPeuckerRecr(left, eps);
        vector<Point> rightPolyline = douglasPeuckerRecr(right, eps);
        leftPolyLine.insert(leftPolyLine.end(), rightPolyline.begin(), rightPolyline.end());
        return leftPolyLine;
    } else {
        // cout << "Begin and end" << endl;
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
        /*
        if (i % 10 == 1) {
            putText(drawing, buffer, rectPoints[0], FONT_HERSHEY_PLAIN, 0.8, Scalar(255, 255, 0));
        }
         */
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
