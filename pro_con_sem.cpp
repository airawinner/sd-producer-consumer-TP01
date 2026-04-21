#include <iostream>
#include <vector>
#include <thread>
#include <semaphore>
#include <random>
#include <atomic>
#include <chrono>
#include <fstream>
#include <mutex>
#include <algorithm>

using namespace std;

const int M = 100000; // Quantidade total de consumos por teste

class Buffer {
private:
    vector<int> data;
    int size;
    int in = 0, out = 0;
    int currentSize = 0;
    mutex mtx;

public:
    counting_semaphore<> sem_empty;
    counting_semaphore<> sem_full;

    Buffer(int n)
        : data(n), size(n), sem_empty(n), sem_full(0) {}

    void insert(int value) {
        sem_empty.acquire();
        lock_guard<mutex> lock(mtx);

        data[in] = value;
        in = (in + 1) % size;
        currentSize++;

        sem_full.release();
    }

    int remove() {
        sem_full.acquire();
        lock_guard<mutex> lock(mtx);

        int value = data[out];
        out = (out + 1) % size;
        currentSize--;

        sem_empty.release();
        return value;
    }

    int getSize() {
        lock_guard<mutex> lock(mtx);
        return currentSize;
    }
};

bool isPrime(int n) {
    if (n < 2) return false;
    if (n % 2 == 0) return n == 2;
    for (int i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

atomic<int> consumedCount(0);
atomic<bool> running(true);

// Produtor: para quando atingir M ou quando running for false
void producer(Buffer& buffer) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 10000000);

    while (running) {
        if (consumedCount >= M) break;
        buffer.insert(dist(gen));
    }
}

// Consumidor: Cada thread tem seu próprio vetor localOccupancy para evitar corrupção de memória
void consumer(Buffer& buffer, vector<int>& localOccupancy) {
    // Reserva espaço para evitar realocações frequentes
    localOccupancy.reserve(M / 2); 

    while (true) {
        int num = buffer.remove();

        // Sentinela de encerramento
        if (num == -1) break;

        isPrime(num);
        
        int current = ++consumedCount;
        
        // Coleta ocupação de forma segura (vetor local da thread)
        localOccupancy.push_back(buffer.getSize());

        if (current >= M) {
            running = false; 
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Uso: ./programa <N> <Np> <Nc>" << endl;
        return 1;
    }

    int N = stoi(argv[1]);
    int Np = stoi(argv[2]);
    int Nc = stoi(argv[3]);

    // Remova os loops externos de N e configs, deixe apenas o das 10 execuções
    double totalTime = 0;
    vector<int> final_occupancy;

    for (int run = 0; run < 10; run++) {
        Buffer buffer(N);
        consumedCount = 0;
        running = true;

        vector<vector<int>> allOccupancies(Nc);
        vector<thread> producers, consumers;

        auto start = chrono::high_resolution_clock::now();

        for (int i = 0; i < Nc; i++)
            consumers.emplace_back(consumer, ref(buffer), ref(allOccupancies[i]));

        for (int i = 0; i < Np; i++)
            producers.emplace_back(producer, ref(buffer));

        for (auto& t : producers) t.join();
        for (int i = 0; i < Nc; i++) buffer.insert(-1);
        for (auto& t : consumers) t.join();

        auto end = chrono::high_resolution_clock::now();
        totalTime += chrono::duration<double>(end - start).count();

        if (run == 9) {
            for (auto& v : allOccupancies)
                final_occupancy.insert(final_occupancy.end(), v.begin(), v.end());
        }
    }

    // Apenas imprime a média para o Python ler
    double avg = totalTime / 10.0;
    cout << avg << endl;

    // Salva o arquivo de ocupação para o Python plotar
    string filename = "ocupacao_N" + to_string(N) + "_Np" + to_string(Np) + "_Nc" + to_string(Nc) + ".txt";
    ofstream file(filename);
    for (int v : final_occupancy) file << v << "\n";

    return 0;
}