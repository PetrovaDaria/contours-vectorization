//
// Created by Dariya Petrova on 08.06.2020.
//

#include "experiments.h"

extern Scalar myWhite;

void processing() {
    String dirPath = "../oneBuilding3/";
    String inputPath = dirPath + "oneBuilding3.jpg";

//    String dirPath = "../satImg/";
//    String inputPath = dirPath + "satImg.jpg";

    String outPutImgPath = dirPath + "result.jpg";
    String paramsPath = dirPath + "result.txt";

    Parameters params = Parameters();

//    // максимальная разница площадей области внутри контура и прямоугольника, описывающего его
//    int maxAreaDiff = 200;
//
//    // параметр размера коридора для Дугласа-Пекера
//    int dpEps = 3;
//
//    // параметры для сетки в алгоритме Грибова
//    int gridStartX = 0;
//    int gridStartY = 0;
//
//    // одинаковый grid interval для x и y
//    int gridInterval = 1;
//
//    // количество предыдущих точек, рассматриваемых в качестве потенциальной лучшей предыдущей точки
//    int prevPointsCount = 2;

    // открываем исходное изображение
    Mat img = imread(inputPath);

    // это изображение только для контуров
    Mat contoursImg;
    Canny(img, contoursImg, 100, 255);

    // ищем на картинке контуры
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(contoursImg, contours, hierarchy,RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    Mat resultImg = Mat::zeros(img.size(), CV_8UC3);

    ofstream currentOut;
    currentOut.open(paramsPath);

    // записываем ширину и высоту
    currentOut << img.cols << endl;
    currentOut << img.rows << endl;

    // записываем количество контуров
    currentOut << contours.size() << endl;

    vector<vector<Point>> allRightContours;

    for (vector<Point> contour: contours) {
        bool isClosed = isClosedContour(contour);
        if (!isClosed) {
            contour = doubleContourToSingle(contour);
        }

        // записываем количество вершин в исходном контуре
        currentOut << contour.size() << endl;

        // записываем координаты каждой вершины исходного контура
        for (Point point: contour) {
            currentOut << point.x << " " << point.y << endl;
        }

        drawLines(resultImg, contour, myGreen);

        // проверяем, близок ли контур по площади к площади описывающего его прямоугольника
        if (canBeDescribedByRect(contour, params.maxAreaDiff)) {
            // находим контур-прямоугольник
            vector<Point> resultContour = processingMinAreaRect(contour);

            // записываем количество вершин в итоговом контуре
            currentOut << resultContour.size() << endl;

            // записываем координаты каждой вершины итогового контура
            for (const Point &point: resultContour) {
                currentOut << point.x << " " << point.y << endl;
            }

            allRightContours.push_back(resultContour);
            drawLines(resultImg, resultContour, myPink);

            // переходим к следующему контуру
            continue;
        }

        // если нельзя описать, как прямоугольник, то используем алгоритм Грибова
        vector<Point> gribovContour = processingGribovAlgorithm(
                resultImg,
                contour,
                params.dpEps,
                params.gridStartPoint.x,
                params.gridStartPoint.y,
                params.gridInterval,
                params.gridInterval,
                params.prevPointsCount);

        // записываем количество вершин в контуре из Грибова
        currentOut << gribovContour.size() << endl;

        // записываем координаты каждой вершины контура из Грибова
        for (const Point &point: gribovContour) {
            currentOut << point.x << " " << point.y << endl;
        }

        allRightContours.push_back(gribovContour);
        drawLines(resultImg, gribovContour, myPink);
    }
    Mat rightImg = Mat::zeros(img.size(), CV_8UC3);
    fillPoly(rightImg, allRightContours, myWhite);
    double iou = getIou(img, rightImg);
    cout << "iou " << iou << endl;
    imwrite(outPutImgPath, resultImg);
}

void iterProcessing() {
//    String dirPath = "../satImg/";
//    String inputPath = dirPath + "satImg.jpg";

    String dirPath = "../netBuildings/";
    String inputPath = dirPath + "netBuildings.jpg";

    String imgPath = dirPath + "img/";
    String paramsPath = dirPath + "params/";


    // максимальная разница площадей области внутри контура и прямоугольника, описывающего его
    int maxAreaDiff = 200;

    // параметр размера коридора для Дугласа-Пекера
    vector<int> dpEpsilons = {1, 2, 3};

    // параметры для сетки в алгоритме Грибова
    int gridStartX = 0;
    int gridStartY = 0;

    // одинаковый grid interval для x и y
    vector<int> gridIntervals = {1, 2, 3};

    // количество предыдущих точек, рассматриваемых в качестве потенциальной лучшей предыдущей точки
    vector<int> prevPointsCounts = {2, 3};

    // открываем исходное изображение
    Mat img = imread(inputPath);

    // это изображение только для контуров
    Mat contoursImg;
    Canny(img, contoursImg, 100, 255);

    // ищем на картинке контуры
    vector<vector<Point>> contours;
    findContours(contoursImg, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // итерируемся по разным значениям, чтобы найти лучшее
    for (int dpEps: dpEpsilons) {
        for (int gridInterval: gridIntervals) {
            for (int prevPointsCount: prevPointsCounts) {
                Mat resultImg = Mat::zeros(img.size(), CV_8UC3);
                String currentPath =
                        "eps_" +
                        to_string(dpEps) +
                        "_interval_" +
                        to_string(gridInterval) +
                        "_ppc_" +
                        to_string(prevPointsCount);
                String currentImagePath = imgPath + currentPath + ".jpg";
                String currentParametersPath = paramsPath + currentPath + ".txt";

                ofstream currentOut;
                currentOut.open(currentParametersPath);

                // записываем ширину и высоту
                currentOut << img.cols << endl;
                currentOut << img.rows << endl;

                // записываем количество контуров
                currentOut << contours.size() << endl;

                vector<vector<Point>> allRightContours;

                for (vector<Point> contour: contours) {
                    bool isClosed = isClosedContour(contour);
                    if (!isClosed) {
                        contour = doubleContourToSingle(contour);
                    }

                    // записываем количество вершин в исходном контуре
                    currentOut << contour.size() << endl;

                    // записываем координаты каждой вершины исходного контура
                    for (Point point: contour) {
                        currentOut << point.x << " " << point.y << endl;
                    }

                    drawLines(resultImg, contour, myGreen);

                    // проверяем, близок ли контур по площади к площади описывающего его прямоугольника
                    if (canBeDescribedByRect(contour, maxAreaDiff)) {
                        // находим контур-прямоугольник
                        vector<Point> resultContour = processingMinAreaRect(contour);

                        // записываем количество вершин в итоговом контуре
                        currentOut << resultContour.size() << endl;

                        // записываем координаты каждой вершины итогового контура
                        for (const Point &point: resultContour) {
                            currentOut << point.x << " " << point.y << endl;
                        }

                        drawLines(resultImg, resultContour, myPink);
                        allRightContours.push_back(resultContour);

                        // переходим к следующему контуру
                        continue;
                    }

                    // если нельзя описать, как прямоугольник, то используем алгоритм Грибова
                    vector<Point> gribovContour = processingGribovAlgorithm(
                            resultImg,
                            contour,
                            dpEps,
                            gridStartX,
                            gridStartY,
                            gridInterval,
                            gridInterval,
                            prevPointsCount);

                    // записываем количество вершин в контуре из Грибова
                    currentOut << gribovContour.size() << endl;

                    // записываем координаты каждой вершины контура из Грибова
                    for (const Point &point: gribovContour) {
                        currentOut << point.x << " " << point.y << endl;
                    }

                    drawLines(resultImg, gribovContour, myPink);
                    allRightContours.push_back(gribovContour);
                }
                Mat rightImg = Mat::zeros(img.size(), CV_8UC3);
                fillPoly(rightImg, allRightContours, myWhite);
                double iou = getIou(img, rightImg);
                cout << "eps " << dpEps << " interval " << gridInterval << " ppc " << prevPointsCount << " iou " << iou << endl;
                imwrite(currentImagePath, resultImg);
            }
        }
    }
}
