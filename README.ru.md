<div align="center">

# Sched Compare

**Симулятор планировщиков Linux CFS vs EEVDF**

</div>

Тактовый симулятор, сравнивающий алгоритмы планирования Linux CFS (Completely Fair Scheduler) и EEVDF (Earliest Eligible Virtual Deadline First). Считывает трассы задач из CSV, запускает оба планировщика с настраиваемой гранулярностью тиков и выдаёт CSV-поток событий планирования (arrival, start, preempt, finish) для анализа. Включает Python-скрипты для построения диаграмм Ганта и CDF-графиков времени ожидания.

## ■ Возможности

- ❖ **Реализация CFS** — справедливое планирование на основе виртуального времени выполнения с поддержкой nice-уровней
- ❖ **Реализация EEVDF** — планирование по принципу earliest eligible virtual deadline first с поддержкой latency-nice
- ❖ **Входные данные в формате CSV** — задачи задаются именем, временем прихода, длительностью, nice и latency-nice
- ❖ **Встроенные сценарии** — пять готовых трасс: `bursty`, `fairness`, `latency_nice`, `mixed`, `nice_weights`
- ❖ **Настраиваемая симуляция** — регулируемое максимальное время, гранулярность тиков и выходной файл
- ❖ **Диаграммы Ганта** — визуализация временных линий планирования с помощью `plot_gantt.py`
- ❖ **CDF времени ожидания** — сравнение кумулятивного распределения с помощью `plot_wait_cdf.py`

## ■ Стек

<div align="center">

| Компонент | Технология |
|-----------|------------|
| Симулятор | C++17 |
| Построение графиков | Python, matplotlib, pandas, numpy |
| Сборка | Make |

</div>

## ■ Как это работает

```
1. Опишите трассу задач в формате CSV: каждая строка задаёт имя, время прихода, длительность, nice и latency-nice.
2. Соберите симулятор через Make и запустите его, выбрав алгоритм cfs или eevdf.
3. Симулятор продвигается по настраиваемым тикам, применяет выбранную политику планирования и записывает события (arrival, start, preempt, finish) в выходной CSV.
4. Запустите plot_gantt.py для отрисовки параллельной диаграммы Ганта обоих планировщиков.
5. Запустите plot_wait_cdf.py для сравнения кумулятивных распределений времени ожидания по трассам.
```

## ■ Скриншоты

<div align="center">

![Screenshot](screenshots/main.png)
*Временная линия планирования и сравнение распределения времени ожидания*

</div>

## ■ Использование

```bash
# Сборка симулятора
cd sim && make

# Запуск симуляции (algo — cfs или eevdf; встроенные трассы находятся в traces/)
./sim/sim cfs traces/mixed.csv --max-time 200 --out results/cfs_mixed.csv
./sim/sim eevdf traces/mixed.csv --max-time 200 --out results/eevdf_mixed.csv

# Построение графиков (скрипты читают CSV результатов через флаги, с разумными значениями по умолчанию)
python3 plots/plot_gantt.py --cfs results/cfs_mixed.csv --eevdf results/eevdf_mixed.csv
python3 plots/plot_wait_cdf.py --results-dir results --traces bursty latency_nice
```
