# Общие сведения

Oil-production-optimization - бакалаврская работа в области оптимизации добычи нефти с целью её максимизации.

Используемые технологии:
1. STL
2. Matplotlib (библиотека Python для простой и удобной визуализации результатов)
3. LaTeX

В проекте реализован алгоритм оптимизации положения скважин (как добывающих нефть, так и нагнетающих воду). 
Текст бакалаврской работы, где подробно описана разработанная программа, находится в папке diploma.
Изменение параметров программы происходит в res/input. 
Визуализировать результаты можно с помощью программ в src/visualization.

## Алгоритм динамического изменения области допустимых значений параметров

Оптимизируемые параметры в данной работе - центры скважин. 
Для каждой скважины задана прямоугольная область, где она может перемещаться в процессе оптимизации. 

Разработанный алгоритм корректирует области скважин так, чтобы они не пересекались:

![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/ver_pic_3.png)


![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/ver_pic_4.png)


## Решение оптимизационной задачи

Решение - получение оптимального положения скважин, при котором добыча нефти максимальна. 
Полученное решение даёт прибавку вплоть до 20% по сравнению со стандартными схемами расположения скважин. 

Можно отследить движение скважин в процессе оптимизации:

![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/Visualize.png)

Или оценить эффект оптимизации в отдельной скважине: 

![Image alt](https://github.com/yabaranov/Oil-production-optimization/raw/master/res/graph/res_pic_Sq_5_2.png)
