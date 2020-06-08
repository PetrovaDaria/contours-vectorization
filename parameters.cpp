//
// Created by Dariya Petrova on 08.06.2020.
//

#include "parameters.h"

class Parameters {
public:
    Parameters() = default;
    // максимальная разница площадей области внутри контура и прямоугольника, описывающего его
    int maxAreaDiff = 200;
    // параметр размера коридора для Дугласа-Пекера
    int dpEps = 3;
    // точка начала сетки в алгоритме Грибова
    Point gridStartPoint = Point(0, 0);
    // одинаковый grid interval для x и y
    int gridInterval = 2;
    // количество предыдущих точек, рассматриваемых в качестве потенциальной лучшей предыдущей точки
    int prevPointsCount = 2;
};
