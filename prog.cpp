#include <iostream>
#include <pthread.h>
#include <vector>
#include <cstring>

// время, необходимое на переход от одного квадрата к другому
#define WALK_TIME 0.000001

// структура, описывающая один квадрат участка
struct Cell {
    char who_completed;
    pthread_mutex_t mutex;
};

// глобальные переменные, описывающие размеры поля по вертикали, горизонтали, скорости рабочих и сад
int n, m;
double worker1_speed, worker2_speed;
std::vector<std::vector<Cell>> garden;

void wait(double x) {
    time_t start_time = clock();
    while ((double) (clock() - start_time) / CLOCKS_PER_SEC < x);
}

void *print_garden() {
    for (int i = 0; i < garden.size(); ++i) {
        for (int j = 0; j < garden[0].size(); ++j) {
            std::cout << garden[i][j].who_completed;
            if (j != garden[0].size()) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
    return nullptr;
}

void do_work(int i, int j, char number) {
    // блокируем другому садовнику вход в квадрат и начинаем разбираться с ней
    pthread_mutex_lock(&garden[i][j].mutex);

    // если квадрат ещё не обработан, то обрабатываем, помечая квадрат своим номером и тратя время,
    // которое соответствует нашему времени работы
    if (garden[i][j].who_completed == '0') {
        if (number == '1') {
            wait(worker1_speed);
        } else {
            wait(worker2_speed);
        }
        garden[i][j].who_completed = number;
    }

    pthread_mutex_unlock(&garden[i][j].mutex);
}

void *worker1(void *vptr_args) {
    // цикл обхода всего участка
    for (int i = 0; i < garden.size(); ++i) {
        // данное ветвление нужно, чтобы корректировать направление движение садовника - влево или
        // вправо
        if (i % 2 == 0) {
            // двигаемся по строчке
            for (int j = 0; j < garden[0].size(); ++j) {
                // "выполняем работу", а затем какое-то время идём к следующему квадрату
                do_work(i, j, '1');
                wait(WALK_TIME);
            }
        } else {
            for (int j = garden[0].size() - 1; j > -1; --j) {
                // "выполняем работу", а затем какое-то время идём к следующему квадрату
                do_work(i, j, '1');
                wait(WALK_TIME);
            }
        }
    }
    return nullptr;
}

void *worker2(void *vptr_args) {
    // данное функция функции worker1 с поправкой на поведение второго садовника
    for (int j = garden[0].size() - 1; j > -1; --j) {
        if ((garden[0].size() - 1 - j) % 2 == 0) {
            for (int i = garden.size() - 1; i > -1; --i) {
                do_work(i, j, '2');
                wait(WALK_TIME);
            }
        } else {
            for (int i = 0; i < garden.size(); ++i) {
                do_work(i, j, '2');
                wait(WALK_TIME);
            }
        }
    }
    return nullptr;
}

void make_obstacles() {
    srand(time(nullptr));

    // генерируем случайное целое
    // число в интервале [0.1 * area of garden; 0.3 * area of garden]
    int bad_cells = ((10.0 + (rand() % 20)) / 100) * (n * m);
    int i, j;

    // раскидываем камни и пруды в случайном порядке по участку, помечаем символом !
    while (bad_cells > 0) {
        i = rand() % n;
        j = rand() % m;

        if (garden[i][j].who_completed == '0') {
            garden[i][j].who_completed = '!';
            --bad_cells;
        }
    }
}

void random_way() {
    srand(time(nullptr));
    n = 1 + rand() % 24;
    m = 1 + rand() % 24;
    worker1_speed = 1000 + (rand() % 1000000);
    worker2_speed = 1000 + (rand() % 1000000);
}

int main(int argc, char **argv) {
    FILE *fin = nullptr;
    FILE *fout = nullptr;
    if (argc == 1) {
        // считываем размеры участка земли и скорости рабочих
        std::cin >> n >> m >> worker1_speed >> worker2_speed;
    } else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
        random_way();
        fin = fopen("tests/rand_input.txt", "w");
        fprintf(fin, "%d %d %lf %lf", n, m, worker1_speed, worker2_speed);
        fout = fopen("tests/rand_output.txt", "w");
    } else if (argc == 3) {
        fin = fopen(argv[1], "r");
        fout = fopen(argv[2], "w");

        if (fin == nullptr) {
            std::cout << "Bad input file\n";
            return 0;
        }

        fscanf(fin, "%d %d %lf %lf\n", &n, &m, &worker1_speed, &worker2_speed);
        fclose(fin);
    } else if (argc == 5) {
        n = std::stoi(argv[1]);
        m = std::stoi(argv[2]);
        worker1_speed = std::stod(argv[3]);
        worker2_speed = std::stod(argv[4]);
    }

    worker1_speed = 1.0 / worker1_speed;                  // настраиваем скорости
    worker2_speed = 1.0 / worker2_speed;

    garden.resize(n);                                     // начинаем настройку сада

    for (int i = 0; i < n; ++i) {
        garden[i].resize(m);
        for (int j = 0; j < m; ++j) {
            garden[i][j].who_completed = '0';                     // инициализируем мьютексы
            // каждого квадрата участка и
            // начальное положение квадрата
            // ('0' - не обработан)
            pthread_mutex_init(&garden[i][j].mutex, nullptr);
        }
    }

    make_obstacles();                                             // добавляем пруды и камни,
    // заканчиваем настройку и
    // выводим начальное состояние
    // сада

    print_garden();
    std::cout << std::endl;

    pthread_t gardener1;
    pthread_t gardener2;

    // запускаем выполнение наших садовников
    pthread_create(&gardener1, nullptr, worker1, nullptr);
    pthread_create(&gardener2, nullptr, worker2, nullptr);

    // обязательно дожидаемся завершения обоих потоков, перед тем как удалять мьютексы, которые
    // могут быть использованы этими потоками
    pthread_join(gardener1, nullptr);
    pthread_join(gardener2, nullptr);

    for (int i = 0; i < garden.size(); ++i) {
        for (int j = 0; j < garden[i].size(); ++j) {
            pthread_mutex_destroy(&garden[i][j].mutex);
        }
    }

    print_garden();

    if (fout != nullptr) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                fprintf(fout, "%c", garden[i][j].who_completed);
                if (j != garden[0].size()) {
                    fprintf(fout, " ");
                }
            }
            fprintf(fout, "\n");
        }
        fclose(fout);
    }
    return 0;
}
