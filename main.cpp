#include <iostream>
#include <queue>
#include <tuple>
#include <vector>
#include <random>
#include <fstream>

using namespace std;

namespace param {
    int Tk = 1; // время передачи
    int Tdp = 1; // время обработки
    int L = 64; // кол-во преамбул
    double Pe = 0.1; // вероятность ошибки
    int k = 3;
    auto N = 100; // кол-во устройств
    auto l = 0.03; // интенсивность входного потока сообщения
    auto T = 1000000; // время моделирования
    int max_arq = 3;
}

class msg {
public:
    int arr_t = 0; // время прибытия
    int recive_t = 0; // время доставки
    int count_arq = 0; // кол-во отправлений

    msg(int time);
};

msg::msg(int time) {
    arr_t = time;
}

tuple<double, double> modeling() {
    int count_errors = 0;
    vector<queue<msg>> queues(param::N);
    int t = 0; // текущее время
    int k_suc = 0;
    double delay_d = 0.0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    vector<pair<int, int>> preambles(param::L);
    while (t < param::T) {
        fill(preambles.begin(), preambles.end(), make_pair(0, 0));
        for (int i = 0; i < param::N; ++i) {
            if (!queues[i].empty()) {
                queues[i].front().count_arq++;
                if (queues[i].front().count_arq > param::max_arq) {
                    queues[i].pop();
                    count_errors++;
                }
                if (queues[i].empty()) {
                    continue;
                }
                int r = rand() % 64;
                preambles[r].first = i; // номер устройства
                preambles[r].second++; // кол-во устройст выбравших преамбулу
            }
        }
        for (auto i = 0; i < param::L; ++i) {
            if (preambles[i].second == 1) {
                auto random = dis(gen);
                if (random > pow(param::Pe, param::k)) {
                    auto m = queues[preambles[i].first].front();
                    double d = t - m.arr_t + (param::Tk * param::k + param::Tdp * 2); // задержка
                    k_suc++;
                    delay_d += d;
                    queues[preambles[i].first].pop();
                }
            }
        }
        double time = param::k * param::Tk + param::Tdp * 2;
        poisson_distribution<> d(param::l);
        for (auto i = 1; i <= time; ++i) {
            for (int j = 0; j < param::N; ++j) {
                auto tmp = d(gen);
                for (int m = 0; m < tmp; ++m) {
                    queues[j].push(msg(t + i));
                }

            }
        }
        t += time;
    }
    double res_P = (double) count_errors / (count_errors + k_suc);
    return make_tuple(delay_d / k_suc, res_P);
}


tuple<double, double> modeling_nc() {
    int count_errors = 0;
    vector<queue<msg>> queues(param::N);
    int t = 0; // текущее время
    int k_suc = 0;
    double delay_d = 0.0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    vector<pair<int, int>> preambles(param::L);
    while (t < param::T) {
        fill(preambles.begin(), preambles.end(), make_pair(0, 0));
        for (int i = 0; i < param::N; ++i) {
            if (!queues[i].empty()) {
                queues[i].front().count_arq++;
                if (queues[i].front().count_arq > param::max_arq) {
                    queues[i].pop();
                    count_errors++;
                }
                if (queues[i].empty()) {
                    continue;
                }
                int r = rand() % 64;
                preambles[r].first = i; // номер устройства
                preambles[r].second++; // кол-во устройст выбравших преамбулу
            }
        }
        for (auto i = 0; i < param::L; ++i) {
            if (preambles[i].second == 1) {
                auto random = dis(gen);
                auto pe = 0.00075;
                if (random > pe) {
                    auto m = queues[preambles[i].first].front();
                    double d = t - m.arr_t + (param::Tk * param::k + param::Tdp * 2); // задержка
                    k_suc++;
                    delay_d += d;
                    queues[preambles[i].first].pop();
                }
            }
        }
        double time = (param::k - 1) * param::Tk + param::Tdp * 2;
        poisson_distribution<> d(param::l);
        for (auto i = 1; i <= time; ++i) {
            for (int j = 0; j < param::N; ++j) {
                auto tmp = d(gen);
                for (int m = 0; m < tmp; ++m) {
                    queues[j].push(msg(t + i));
                }

            }
        }
        t += time;
    }
    double res_P = (double) count_errors / (count_errors + k_suc);
    return make_tuple(delay_d / k_suc, res_P);
}


int main() {
    string filename1 = "krep.dat";
    string filename2 = "nc.dat";
    string filename3 = "krep_err.dat";
    string filename4 = "nc_err.dat";
    ofstream out1(filename1);
    ofstream out2(filename2);
    ofstream out3(filename3);
    ofstream out4(filename4);
    for (int n = 5; n <= 200; n += 5) {
        param::N = n;
        cout << n << endl;

        auto [d, err] = modeling();
        out1 << n << ' ' << d << endl;
        out3 << n << ' ' << err << endl;
        auto [d2, err2] = modeling_nc();
        out2 << n << ' ' << d2 << endl;
        out4 << n << ' ' << err2 << endl;
    }
}
