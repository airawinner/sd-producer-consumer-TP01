#include <iostream>
#include <thread>
#include <vector>
#include <semaphore>
#include <mutex>
#include <cstdlib>
#include <chrono>

using namespace std;

#define PR 1   // número de produtores
#define CN 1   // número de consumidores
#define N 5    // tamanho do buffer

int buffer[N] = {0};
int index_in = 0;
int index_out = 0;

// 🔥 nomes corrigidos (sem conflito com std::empty)
counting_semaphore<N> sem_empty(N);
counting_semaphore<N> sem_full(0);

// mutex
mutex mtx;

// produtor
void produtor(int id) {
    while (true) {
        int item = rand() % 5;

        sem_empty.acquire();

        mtx.lock();
        buffer[index_in] = item;

        cout << "Produtor " << id
             << " inseriu " << item
             << " em buffer[" << index_in << "]\n";

        index_in = (index_in + 1) % N;
        mtx.unlock();

        sem_full.release();

        this_thread::sleep_for(chrono::seconds(rand() % 3));
    }
}

// consumidor
void consumidor(int id) {
    while (true) {
        sem_full.acquire();

        mtx.lock();
        int item = buffer[index_out];

        cout << "Consumidor " << id
             << " removeu " << item
             << " de buffer[" << index_out << "]\n";

        index_out = (index_out + 1) % N;
        mtx.unlock();

        sem_empty.release();

        this_thread::sleep_for(chrono::seconds(rand() % 6));
    }
}

int main() {
    cout << "BUFFER INICIALIZADO!\n";

    vector<thread> produtores;
    vector<thread> consumidores;

    // cria produtores
    for (int i = 0; i < PR; i++)
        produtores.emplace_back(produtor, i);

    // cria consumidores
    for (int i = 0; i < CN; i++)
        consumidores.emplace_back(consumidor, i);

    // join (programa roda infinito)
    for (auto& t : produtores) t.join();
    for (auto& t : consumidores) t.join();

    return 0;
}