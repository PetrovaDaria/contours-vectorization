//
// Created by Dariya Petrova on 11.05.2020.
//

#include <iostream>
#include "drawing.h"

/*!
 * показывает изображение
 * @param img - изображение
 * @param imgName - имя изображения
 */
void showImg(Mat img, String imgName) {
    imshow(imgName, img);
    waitKey(0);
}

/*!
 * рисует на изображении линии
 * @param img - изображение
 * @param points - массив точек
 * @param color - цвет линии
 * @param joinEnds - соединяются ли концы
 * @param isShowImg - надо ли показывать изображение
 * @param imgName - имя изображения
 */
void drawLines(Mat img, vector<Point> points, Scalar color, bool joinEnds, bool isShowImg,
               String imgName) {
    int size = points.size();
    int end = size;
    if (!joinEnds) {
        end -= 1;
    }
    for (int j = 0; j < end; j++) {
        line(img, points[j], points[(j + 1) % size], color, 1, LINE_AA);
        if (isShowImg) {
            showImg(img, imgName);
        }
    }
}

/*!
 * рисует на изображении точки
 * @param img - изображение
 * @param points - массив точек
 * @param color - цвет точек
 */
void drawPoints(Mat img, vector<Point> points, Scalar color) {
    int size = points.size();
    for (int j = 0; j < size; j++) {
        line(img, points[j], points[j], color);
    }
}

/*!
 * обрезка картинки
 */
void cropImg(String imgPath, String newImgPath, int startX, int startY, int width, int height) {
//    String imgPath = "../netBuildingsMarking.png";
//    String newImgPath = "../netBuildingsMarking.jpg";

    Mat image = imread(imgPath);

    Mat ROI(image, Rect(startX, startY, width, height));

    Mat croppedImage;

    ROI.copyTo(croppedImage);

    imwrite(newImgPath, croppedImage);
}

void printPoints(vector<Point> points) {
    for (Point point: points) {
        cout << point.x << ";" << point.y << endl;
    }
}
