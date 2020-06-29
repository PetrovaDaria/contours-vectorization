#include <iostream>
#include "main.h"
#include <ctime>

Scalar myRed(Scalar(0, 0, 255));
Scalar myGreen(Scalar(0, 255, 0));
Scalar myBlue(Scalar(255, 0, 0));
Scalar myPink(Scalar(255, 0, 255));
Scalar myWhite(Scalar(255, 255, 255));
Scalar myWhite2(Scalar(255, 255, 255, 128));
Scalar myGrey(Scalar(128, 128, 128, 0.5));

int main() {
    Mat img = imread("../examples/cn_2500_1000_500_500/cn_2500_1000_500_500.jpg");
    Mat markedImg = imread("../examples/cn_2500_1000_500_500/cnMarking_2500_1000_500_500.png");
    vectorizeImgWithSeveralParameters(img, markedImg, {1}, {1, 2}, {2}, "../newDir/");
}

/*!
 * векторизуем изображение с одинарным комплектом параметров
 * @param img - исходное изображение
 * @param img - изображение того же участка, но с ручной разметкой
 * @param dpEps - параметр удаленности для алгоритма Дугласа-Пекера
 * @param gridInterval - интервал между точками вспомогательной сетки алгоритма Грибова
 * @param ppc - количество рассматриваемых предыдущих точек в алгоритме Грибова
 * @param dirPath - путь к папке, в которой будут лежать результаты (на конце слэш) (папка уже должна быть создана)
 * @return векторизованные контуры
 */
vector<vector<Point>> vectorizeImg(const Mat& img, const Mat& markedImg, int dpEps, int gridInterval, int ppc, const String& dirPath) {
    String commonPathPart = "_dpEps_" +  to_string(dpEps) + "_interval_" + to_string(gridInterval) + "_ppc_" + to_string(ppc) + ".jpg";
    String resultImgPath = dirPath + commonPathPart;
    String onlyContoursImgPath = dirPath + "only_contours" + commonPathPart;
    String resultWithMarkingPath = dirPath + "with_marking" + commonPathPart;
    String iouPath = dirPath + "iou.txt";
    String timePath = dirPath + "time.txt";

    ofstream iouOut;
    iouOut.open(iouPath, ofstream::out);
    iouOut << "Initial IoU " << getIou(img, markedImg) << endl;

    ofstream timeOut;
    timeOut.open(timePath, ofstream::out);

    // они не сильно влияют, пусть будут дефолтными
    int gridStartX = 0;
    int gridStartY = 0;

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

    timeOut << "Количество контуров " << contours.size() << endl;

    Mat resultImg = Mat::zeros(img.size(), CV_8UC3);
    Mat resultImgWithMarking = markedImg.clone();

    vector<vector<Point>> vectorizedContours;

    int startTime = clock();

    for (vector<Point> contour: contours) {
        drawLines(resultImgWithMarking, contour, myPink);
        drawLines(resultImg, contour, myPink);

        bool isClosed = isClosedContour(contour);
        if (!isClosed) {
            contour = doubleContourToSingle(contour);
        }

        // проверяем, близок ли контур по площади к площади описывающего его прямоугольника
        if (canBeDescribedByRect(contour)) {
            // находим контур-прямоугольник
            vector<Point> resultContour = processingMinAreaRect(contour);

            drawLines(resultImgWithMarking, resultContour, myGreen);
            drawLines(resultImg, resultContour, myGreen);

            vectorizedContours.push_back(resultContour);

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
                ppc);

        drawLines(resultImgWithMarking, gribovContour, myGreen);
        drawLines(resultImg, gribovContour, myGreen);
        vectorizedContours.push_back(gribovContour);
    }
    int endTime = clock();
    // время работы
    double duration = endTime - startTime;
    timeOut << "eps " << dpEps << " interval " << gridInterval << " ppc " << ppc << " duration " << duration << endl;
    timeOut.close();

    Mat rightImg = Mat::zeros(img.size(), CV_8UC3);
    fillPoly(rightImg, vectorizedContours, myWhite);

    // вычисляем iou для ручной разметки и полученного алгоритмом изображения
    double iou = getIou(markedImg, rightImg);
    iouOut << "eps " << dpEps << " interval " << gridInterval << " ppc " << ppc << " iou " << iou << endl;
    iouOut.close();

    imwrite(resultImgPath, rightImg);
    imwrite(onlyContoursImgPath, resultImg);
    imwrite(resultWithMarkingPath, resultImgWithMarking);

    return vectorizedContours;
}

/*!
 * векторизуем изображение с разными комбинациями параметров
 * @param img - исходное изображение
 * @param img - изображение того же участка, но с ручной разметкой
 * @param dpEpss - массив возможных значений параметра удаленности для алгоритма Дугласа-Пекера
 * @param gridIntervals - массив возможных значений интервала между точками сетки в алгоритме Грибова
 * @param ppcs - массив возможных значений количества рассматриваемых предыдущих точек
 * @param dirPath - путь к папке, в которой будут лежать результаты (на конце слэш) (папка уже должна быть создана)
 * @return векторизованные контуры, обладающие наилучшим iou
 */
vector<vector<Point>> vectorizeImgWithSeveralParameters(
        const Mat& img,
        const Mat& markedImg,
        const vector<int>& dpEpss,
        const vector<int>& gridIntervals,
        const vector<int>& ppcs,
        const String& dirPath
) {
    String iouPath = dirPath + "iou.txt";
    String timePath = dirPath + "time.txt";

    ofstream iouOut;
    iouOut.open(iouPath, ofstream::out);
    iouOut << "Initial IoU " << getIou(img, markedImg) << endl;

    ofstream timeOut;
    timeOut.open(timePath, ofstream::out);

    // они не сильно влияют, пусть будут дефолтными
    int gridStartX = 0;
    int gridStartY = 0;

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

    timeOut << "Количество контуров " << contours.size() << endl;

    double bestIou = 0;
    vector<vector<Point>> bestVectorizedContours;

    // итерируемся по разным значениям, чтобы найти лучшее
    for (int dpEps: dpEpss) {
        for (int gridInterval: gridIntervals) {
            for (int ppc: ppcs) {
                String commonPathPart = "_dpEps_"
                        +  to_string(dpEps)
                        + "_interval_"
                        + to_string(gridInterval)
                        + "_ppc_"
                        + to_string(ppc)
                        + ".jpg";
                String resultImgPath = dirPath + commonPathPart;
                String onlyContoursImgPath = dirPath + "only_contours" + commonPathPart;
                String resultWithMarkingPath = dirPath + "with_marking" + commonPathPart;

                Mat resultImg = Mat::zeros(img.size(), CV_8UC3);
                Mat resultImgWithMarking = markedImg.clone();

                vector<vector<Point>> vectorizedContours;

                int startTime = clock();

                for (vector<Point> contour: contours) {
                    drawLines(resultImgWithMarking, contour, myPink);
                    drawLines(resultImg, contour, myPink);

                    bool isClosed = isClosedContour(contour);
                    if (!isClosed) {
                        contour = doubleContourToSingle(contour);
                    }

                    // проверяем, близок ли контур по площади к площади описывающего его прямоугольника
                    if (canBeDescribedByRect(contour)) {
                        // находим контур-прямоугольник
                        vector<Point> resultContour = processingMinAreaRect(contour);

                        drawLines(resultImgWithMarking, resultContour, myGreen);
                        drawLines(resultImg, resultContour, myGreen);

                        vectorizedContours.push_back(resultContour);

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
                            ppc);

                    drawLines(resultImgWithMarking, gribovContour, myGreen);
                    drawLines(resultImg, gribovContour, myGreen);
                    vectorizedContours.push_back(gribovContour);
                }
                int endTime = clock();
                // время работы
                double duration = endTime - startTime;
                timeOut << "eps " << dpEps << " interval " << gridInterval << " ppc " << ppc << " duration " << duration
                        << endl;

                Mat rightImg = Mat::zeros(img.size(), CV_8UC3);
                fillPoly(rightImg, vectorizedContours, myWhite);

                // вычисляем iou для ручной разметки и полученного алгоритмом изображения
                double iou = getIou(markedImg, rightImg);
                iouOut << "eps " << dpEps << " interval " << gridInterval << " ppc " << ppc << " iou " << iou << endl;

                if (iou > bestIou) {
                    bestIou = iou;
                    bestVectorizedContours = vectorizedContours;
                }

                imwrite(resultImgPath, rightImg);
                imwrite(onlyContoursImgPath, resultImg);
                imwrite(resultWithMarkingPath, resultImgWithMarking);
            }
        }
    }
    iouOut.close();
    timeOut.close();

    return bestVectorizedContours;
}
