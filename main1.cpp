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

int main() {
    vector<int> Ns = {1, 10, 100, 1000};
    vector<pair<int, int>> configs = {
        {1, 1}, {1, 2}, {1, 4}, {1, 8},
        {2, 1}, {4, 1}, {8, 1}
    };

    ofstream csv("resultados.csv");
    csv << "N,Np,Nc,tempo\n";

    for (int N : Ns) {
        for (auto [Np, Nc] : configs) {
            double totalTime = 0;
            vector<int> final_occupancy;

            cout << "Executando N=" << N << " Np=" << Np << " Nc=" << Nc << "... " << flush;

            for (int run = 0; run < 10; run++) {
                Buffer buffer(N);
                consumedCount = 0;
                running = true;

                // Lista de vetores: um para cada thread consumidora
                vector<vector<int>> allOccupancies(Nc);
                vector<thread> producers, consumers;

                auto start = chrono::high_resolution_clock::now();

                // 1. Inicia threads consumidoras com seus respectivos vetores locais
                for (int i = 0; i < Nc; i++)
                    consumers.emplace_back(consumer, ref(buffer), ref(allOccupancies[i]));

                // 2. Inicia threads produtoras
                for (int i = 0; i < Np; i++)
                    producers.emplace_back(producer, ref(buffer));

                // 3. Aguarda produtores terminarem
                for (auto& t : producers) t.join();

                // 4. Envia sentinelas para destravar e encerrar consumidores
                for (int i = 0; i < Nc; i++)
                    buffer.insert(-1);

                // 5. Aguarda consumidores terminarem
                for (auto& t : consumers) t.join();

                auto end = chrono::high_resolution_clock::now();
                double elapsed = chrono::duration<double>(end - start).count();
                totalTime += elapsed;

                // No último run, mescla os resultados para salvar no arquivo
                if (run == 9) {
                    final_occupancy.clear();
                    for (auto& v : allOccupancies) {
                        final_occupancy.insert(final_occupancy.end(), v.begin(), v.end());
                    }
                }
            }

            double avg = totalTime / 10.0;
            cout << "Média: " << avg << "s" << endl;

            csv << N << "," << Np << "," << Nc << "," << avg << "\n";

            // Salvar ocupação final em arquivo
            string filename = "ocupacao_N" + to_string(N) + "_Np" + to_string(Np) + "_Nc" + to_string(Nc) + ".txt";
            ofstream file(filename);
            for (int v : final_occupancy)
                file << v << "\n";
        }
    }

    csv.close();
    cout << "\nSimulação finalizada com sucesso!" << endl;

    return 0;
}