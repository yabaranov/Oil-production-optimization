# Oil-production-optimization

Проект представляет собой программу оптимизации положения скважин на С++ и Python.<br>
На основе проекта написана бакалаврская работа.

## Модули проекта
* [Метод Гаусса-Ньютона](#gauss_newton)
* [Корректировка областей скважин](#correction)
* [Результаты](#results)

## Используемые библиотеки:
1. Matplotlib - библиотека для визуализации данных на Python 
2. Eigen - библиотека линейной алгебры на C++ 

## <a id="gauss_newton">Метод Гаусса-Ньютона</a>

Данный метод на каждой итерации вычисляет шаг для координат скважин.<br> 
Таким образом, он постепенно двигает скважины, добиваясь наибольшей добычи нефти. 

## <a id="correction">Корректировка областей скважин</a>

Для каждой скважины задана прямоугольная область, где она может перемещаться. <br>
Разработанный алгоритм корректирует области так, чтобы они не пересекались.<br>

Было:
![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/geometryBefore.png)

Стало:
![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/geometryAfter.png)

## <a id="results">Результаты</a>
Оптимизация даёт прибавку до 20% по сравнению со стандартными схемами расположения скважин. <br>
Можно отследить движение скважин:

![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/wellMovement.png)
где черный контур – контур нефтеносности, круг – добывающая скважина, треугольник – нагнетательная.

Или оценить эффект оптимизации в отдельной скважине: 

![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/productionByDay.png)

