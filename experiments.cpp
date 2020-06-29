//
// Created by Dariya Petrova on 08.06.2020.
//

#include "experiments.h"

extern Scalar myWhite;

int main2() {
    //cropImg("../cn.jpg", "../cn_2500_1000_500_500/cn_2500_1000_500_500.jpg", 2500, 1000, 500, 500);
//     cropImg("../cn.png", "../cn_2500_1000_500_500/cn_2500_1000_500_500.png", 2500, 1000, 500, 500);
//     cropImg("../cnMarking.png", "../cn_2500_1000_500_500/cnMarking_2500_1000_500_500.png", 2500, 1000, 500, 500);
//    iterProcessing("../netBuildings_3000_200_500_500/",
//            "netBuildings_3000_200_500_500.png",
//            "netBuildingsMarking_3000_200_500_500.png");
//    iterProcessing("../fullCn/",
//            "cn.png",
//            "cnMarking.png");
//     iterProcessing("../double/", "double.png", "double.png");
//     tryDiffCnts();
}

void tryDiffCnts() {
    Mat tryImg = imread("../difficult/difficult5.jpg");
    Mat dilatedContoursImg;
    dilate(tryImg, dilatedContoursImg, Mat());

    Mat erodedContoursImg;
    erode(dilatedContoursImg, erodedContoursImg, Mat());

    showImg(erodedContoursImg, "dilate + erode");

    Mat contoursImg;
    Canny(dilatedContoursImg, contoursImg, 100, 255);
    showImg(contoursImg, "contours");

    Mat dilatedContoursImg2;
    dilate(contoursImg, dilatedContoursImg2, Mat());

    Mat erodedContoursImg2;
    erode(dilatedContoursImg2, erodedContoursImg2, Mat());

    showImg(erodedContoursImg2, "dilate + erode contours");

    vector<vector<Point>> externalContours;
    vector<Vec4i> externalHierarchy;
    findContours(erodedContoursImg2, externalContours, externalHierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);
    Mat externalImg = Mat::zeros(tryImg.size(), CV_8UC3);
    for (vector<Point> contour: externalContours) {
        drawLines(externalImg, contour, myGreen);
    }
    showImg(externalImg, "external");

    Mat dpImg = Mat::zeros(tryImg.size(), CV_8UC3);
    for (vector<Point> contour: externalContours) {
        vector<Point> dpContour;
        approxPolyDP(contour, dpContour, 1, true);
        drawLines(dpImg, dpContour, myGreen);
    }
    showImg(dpImg, "dp");
}

void processing() {
    String dirPath = "../difficult/";
    String inputPath = dirPath + "difficult.png";

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
        vector<Point> gribovContour = processingGribovAlgorithm2(
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
    showImg(rightImg, "result");
    showImg(img, "initial");
    fillPoly(rightImg, allRightContours, myWhite);
    double iou = getIou(img, rightImg);
    cout << "iou " << iou << endl;
    imwrite(outPutImgPath, resultImg);
}

void iterProcessing(String dirPath, String inputImgPath, String markingImgPath) {
    String fullInputImgPath = dirPath + inputImgPath;
    String fullMarkingImgPath = dirPath + markingImgPath;

    String imgPath = dirPath + "img/";
    String markingDirPath = dirPath + "marking/";
    String paramsPath = dirPath + "params/";

    String markingParamsPath = dirPath + "markingParams.txt";

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
    Mat img = imread(fullInputImgPath);

    // открываем изображение с ручной разметкой
    Mat markingImg = imread(fullMarkingImgPath);

    // это изображение только для контуров
    Mat markingContoursImg;
    Canny(markingImg, markingContoursImg, 100, 255);

    // ищем на картинке контуры
    vector<vector<Point>> markingContours;
    findContours(markingContoursImg, markingContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    ofstream markingParamsOut;
    markingParamsOut.open(markingParamsPath);
    markingParamsOut << markingImg.cols << endl;
    markingParamsOut << markingImg.rows << endl;
    markingParamsOut << markingContours.size() << endl;
    for (vector<Point> contour: markingContours) {
//        markingParamsOut << contour.size() << endl;
//        for (Point point: contour) {
//            markingParamsOut << point.x << " " << point.y << endl;
//        }

        vector<Point> singled = doubleContourToSingle(contour);
        markingParamsOut << singled.size() << endl;
        for (Point point: singled) {
            markingParamsOut << point.x << " " << point.y << endl;
        }
    }
    markingParamsOut.close();

    // применяем дилатацию к исходному изображению
    Mat dilatedContoursImg;
    dilate(img, dilatedContoursImg, Mat());

    // применяем эрозию к дилатированному изображению
    Mat erodedContoursImg;
    erode(dilatedContoursImg, erodedContoursImg, Mat());

    // ищем контуры на дилатированно-эрозированном изображении
    Mat contoursImg;
    Canny(erodedContoursImg, contoursImg, 100, 255);

    // применяем дилатацию к контурам
    Mat dilatedContoursImg2;
    dilate(contoursImg, dilatedContoursImg2, Mat());

    // применяем эрозию к контурам
    Mat erodedContoursImg2;
    erode(dilatedContoursImg2, erodedContoursImg2, Mat());

    // ищем на картинке контуры
    vector<vector<Point>> contours;
    findContours(erodedContoursImg2, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    cout << "Количество контуров " << contours.size() << endl;

    String iouPath = dirPath + "iou.txt";

    ofstream iouOut;
    iouOut.open(iouPath);

    iouOut << "init iou " << getIou(img, markingImg) << endl;
    cout << "init iou" << getIou(img, markingImg) << endl;

    // итерируемся по разным значениям, чтобы найти лучшее
    for (int dpEps: dpEpsilons) {
        for (int gridInterval: gridIntervals) {
            for (int prevPointsCount: prevPointsCounts) {
                Mat resultImg = Mat::zeros(img.size(), CV_8UC3);
                Mat resultImgWithMarking = markingImg.clone();
                String currentPath =
                        "eps_" +
                        to_string(dpEps) +
                        "_interval_" +
                        to_string(gridInterval) +
                        "_ppc_" +
                        to_string(prevPointsCount);
                String currentImagePathWithMarking = markingDirPath + "Marking" + currentPath + ".jpg";
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

                int startTime = clock();

                for (vector<Point> contour: contours) {
                    drawLines(resultImgWithMarking, contour, myPink);
                    drawLines(resultImg, contour, myGreen);

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

                    // drawLines(resultImg, contour, myGreen);

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
                    vector<Point> gribovContour = processingGribovAlgorithm2(
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
                int endTime = clock();
                double duration = (endTime - startTime) / 1000.0;
                cout << "eps " << dpEps << " interval " << gridInterval << " ppc " << prevPointsCount << " duration " << duration << endl;

                Mat rightImg = Mat::zeros(img.size(), CV_8UC3);
                fillPoly(rightImg, allRightContours, myWhite);

                // fillPoly(resultImgWithMarking, allRightContours, myGrey);
                for (vector<Point> contour: allRightContours) {
                    drawLines(resultImgWithMarking, contour, myGreen);
                }
//                for (vector<Point> contour: allRightContours) {
//                    drawPoints(rightImg, contour, myRed);
//                }
                // showImg(rightImg, "right");
                // double iou = getIou(img, rightImg);
                // вычисляем iou для ручной разметки и полученного алгоритмом изображения
                double iou = getIou(markingImg, rightImg);
                iouOut << "eps " << dpEps << " interval " << gridInterval << " ppc " << prevPointsCount << " iou " << iou << endl;
                cout << "eps " << dpEps << " interval " << gridInterval << " ppc " << prevPointsCount << " iou " << iou << endl;
                imwrite(currentImagePath, resultImg);
                imwrite(currentImagePathWithMarking, resultImgWithMarking);
            }
        }
    }
    iouOut.close();
}
