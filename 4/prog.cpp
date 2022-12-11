#include <iostream>
#include <pthread.h>
#include <vector>

#define WALK_TIME 0.1

int n, m;
double worker1_work_time, worker2_work_time;
std::vector<std::vector<char>> garden;
std::vector<std::vector<pthread_mutex_t>> mutexes;

void wait(double x) {
    time_t start_time = clock();
    while ((double) (clock() - start_time) / CLOCKS_PER_SEC < x);
}

void *print_garden(void* vptr_args) {
    for (int i = 0; i < garden.size(); ++i) {
        for (int j = 0; j < garden[0].size(); ++j) {
            std::cout << garden[i][j];
            if (j != garden[0].size()) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
    return nullptr;
}

void do_work(int i, int j, char number) {
    pthread_mutex_lock(&mutexes[i][j]);

    if (garden[i][j] == '0') {
        if (number == '1') {
            wait(worker1_work_time);
        } else {
            wait(worker2_work_time);
        }
        garden[i][j] = number;
    }

    pthread_mutex_unlock(&mutexes[i][j]);
}

void *worker1(void *vptr_args) {
    for (int i = 0; i < garden.size(); ++i) {
        if (i % 2 == 0) {
            for (int j = 0; j < garden[0].size(); ++j) {
                do_work(i, j, '1');
                wait(WALK_TIME);
            }
        } else {
            for (int j = garden[0].size() - 1; j > -1; --j) {
                do_work(i, j, '1');
                wait(WALK_TIME);
            }
        }
    }
    return nullptr;
}

void *worker2(void *vptr_args) {
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
    int percent = ((10.0 + (rand() % 20)) / 100) * (n * m);
//    std::cout << percent << '\n';
    int i, j;

    while (percent > 0) {
        i = rand() % n;
        j = rand() % m;

        if (garden[i][j] == '0') {
            garden[i][j] = '3';
            --percent;
        }
    }
}

int main() {
    std::cin >> n >> m >> worker1_work_time >> worker2_work_time;

    worker1_work_time = 1.0 / worker1_work_time;
    worker2_work_time = 1.0 / worker2_work_time;

    garden.resize(n);
    mutexes.resize(n);

    for (int i = 0; i < n; ++i) {
        garden[i].resize(m);
        mutexes[i].resize(m);
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            garden[i][j] = '0';
            pthread_mutex_init(&mutexes[i][j], nullptr);
        }
    }

//    make_obstacles();

    print_garden(nullptr);
    std::cout << std::endl;

    pthread_t gardener1;
    pthread_create(&gardener1, nullptr, worker1, nullptr);
    worker2(nullptr);

    pthread_join(gardener1, nullptr);

    for (int i = 0; i < mutexes.size(); ++i) {
        for (int j = 0; j < mutexes[i].size(); ++j) {
            pthread_mutex_destroy(&mutexes[i][j]);
        }
    }

    print_garden(nullptr);
    return 0;
}
