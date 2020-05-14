#include <iostream>

#include "main.h"


Scalar red(Scalar(0, 0, 255));
Scalar green(Scalar(0, 255, 0));
Scalar blue(Scalar(255, 0, 0));

int main() {
    gribovAlgorithm();
    // rotatedMinAreaRect();
    // !!! projection();
    // integratedDP();
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
    findContours(cannyOutput, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
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
        RotatedRect minRect = minAreaRect(contour);
        Rect boundRect = boundingRect(contour);
        int left = boundRect.x;
        int top = boundRect.y;
        int width = boundRect.width;
        int height = boundRect.height;
        int x_end = left + width;
        int y_end = top + height;
        int cntArea = 0;
        for (int x = left; x < x_end; x++)
        {
            for (int y = top; y < y_end; y++)
            {
                double test = pointPolygonTest(contour, Point2f(x, y), false);
                if (test == 1 || test == 0) {
                    cntArea += 1;
                }
            }
        }
        cout << i << endl;
        cout << "Contour size " << contour.size() << endl;
        cout << "Contour moment area " << moments(contour).m00 << endl;
        cout << "Contour area " << cntArea << endl;
        cout << "Rect size " << minRect.size << endl;
        cout << "Rect area " << minRect.size.area() << endl;
        Point2f rectPoints[4];
        minRect.points(rectPoints);
        drawContours(drawing, contours, (int)i, contourColor);
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
