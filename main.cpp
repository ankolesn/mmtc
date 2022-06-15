#include <iostream>
#include <queue>
#include <vector>
#include <random>

using namespace std;

class msg{
public:
    int arr_t = 0; // время прибытия
    int recive_t = 0; // время доставки

    msg(int time);
};

msg::msg(int time) {
    arr_t = time;
}

double modeling(int N, double l, int T){
    int Tk = 1; // время передачи
    int Tdp = 1; // время обработки

    vector<queue<msg>> queues(N);
    int t = 0; // текущее время
    int L = 64; // кол-во преамбул
    double Pe = 0; // вероятность ошибки
    int k = 3;
    int k_suc = 0;
    double delay_d = 0.0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    vector<pair<int, int>> preambles(L);
    while (t < T){
        fill(preambles.begin(), preambles.end(), make_pair(0, 0));
        for (int i = 0; i < N; ++i) {
            if(!queues[i].empty()){
                int r = rand() % 64;
                preambles[r].first = i; // номер устройства
                preambles[r].second++; // кол-во устройст выбравших преамбулу
            }
        }
        for (auto i = 0; i < L; ++i) {
            if (preambles[i].second == 1){
                auto random = dis(gen);
                if(random > pow(Pe, k)) {
                    auto m = queues[preambles[i].first].front();
                    double d = t - m.arr_t + (Tk * k + Tdp * 2); // задержка
                    k_suc++;
                    delay_d += d;
                    queues[preambles[i].first].pop();
                }
            }
        }
        double time = k * Tk + Tdp * 2;
        poisson_distribution<> d(l);
        for (auto i = 1; i <= time; ++i) {
            for (int j = 0; j < N; ++j) {
                auto tmp = d(gen);
                for (int m = 0; m < tmp; ++m) {
                    queues[j].push(msg(t + i));
                }

            }
        }
        t += time;
    }
    return delay_d/k_suc;
}

int main() {
    auto N = 150; // кол-во устройств
    auto l = 0.03; // интенсивность входного потока сообщения
    auto T = 100000; // время модулирования
    cout << modeling(N, l, T)  << endl;


}
