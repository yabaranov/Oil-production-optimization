# Oil-production-optimization

Бакалаврская работа в области оптимизации добычи нефти с целью её максимизации. <br>
В проекте реализован алгоритм оптимизации положения добывающих и нагнетательных скважин. 

## Технологии:
1. STL
2. Eigen
3. CMake
4. Matplotlib (библиотека Python для визуализации данных)
5. RapidJSON

## Описание модулей проекта
Текст бакалаврской работы находится в папке diploma.<br>
Изменение параметров программы происходит в res/input. 

### Алгоритм динамического изменения областей скважин

Для каждой скважины задана прямоугольная область, где она может перемещаться в процессе оптимизации. 

Разработанный алгоритм корректирует области скважин так, чтобы они не пересекались:

![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/geometryBefore.png)


![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/geometryAfter.png)


### Решение

Итоговый результат - получение положения скважин, при котором добыча нефти максимальна. <br>
Оптимизация даёт прибавку до 20% по сравнению со стандартными схемами скважин. 

Можно отследить движение скважин в процессе оптимизации:

![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/wellMovement.png)

Или оценить эффект оптимизации в отдельной скважине: 

![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/productionByDay.png)