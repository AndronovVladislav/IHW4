# ИДЗ4
# Андронов Владислав
## Условие
**Задача о нелюдимых садовниках.** Имеется пустой участок земли (двумерный массив) и план сада, разбитого на отдельные квадраты. От 10 до 30 процентов площади сада заняты
прудами или камнями. То есть недоступны для ухаживания. Эти квадраты располагаются на плане произвольным образом. Ухаживание за садом выполняют два садовника, которые не
хотят встречаться друг другом (то есть, одновременно появляться в одном и том же квадрате). Первый садовник начинает работу с верхнего левого угла сада и перемещается
слева направо, сделав ряд, он спускается вниз и идет в обратном направлении, пропуская обработанные участки. Второй садовник начинает работу с нижнего правого угла сада
и перемещается снизу вверх, сделав ряд, он перемещается влево и также идет в обратную сторону. Если садовник видит, что участок сада уже обработан другим садовником или 
является необрабатываемым, он идет дальше. Если по пути какой-то участок занят другим садовником, то садовник ожидает когда участок освободится, чтобы пройти дальше.
Садовники должны работать одновременно со скоростями, определяемыми как параметры задачи. Прохождение через любой квадрат занимает некоторое время, которое задается
константой, меньшей чем времена обработки и принимается за единицу времени. **Создать многопоточное приложение, моделирующее работу садовников. При решении задачи
использовать мьютексы.**

# На 4
## Описание используемой модели параллельных вычислений
Программа написана по модели итеративного параллелизма, эта модель здесь кажется наиболее подходящей. Потоки представляют собой функции, имеющие внутри себя циклы, сами функции должны взаимодействовать друг с другом - описание работы программы абсолютно совпадает с описанием этой модели.
## Возможные входные данные
2 2 10000 10000

10 10 1000000 1000000

и любые другие корректные входные данные(скорость лучше ставить больше 10000-100000, чтобы не ждать слишком долго)

# На 5
## Log
Добавлены необходимые комментарии
## Сценарий поведения
Cразу после получения плана сада оба садовника стартуют: первый стартует из левого верхнего угла участка, второй - правого нижнего. Если один из садовников приходит в квадрат и, стоя на его краю, обнаруживает, что этим квадратом уже занимается другой садовник, то он терпеливо стоит у самого края конфликтного квадрата, пока оттуда не уйдёт другой садовник и уже затем начинает разбираться, что же оставил после себя этот садовник. Таким образом каждый из садовников обходит весь сад.

# На 6
## Log
Внесены необходимые изменения в код
## Алгоритм
Алгоритм полностью идентичен описанному выше сценарию поведения, однако в роли двух садовников выступают два потока, в роли сада вектор векторов(матрица) квадратов участка, в роли идентификатора наличия другого садовника на квадрате - запертый мьютекс у экземпляра Cell, отвечающего за этот квадрат, в роли края квадрата(где один из садовников ждёт своего более быстрого конкурента) - строчка 39 кода.
