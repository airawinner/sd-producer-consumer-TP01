#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <random>
#include <cmath>

using namespace std;

int main(int argc, char* argv[]) {
    if(argc < 2){
        cerr << "Necessário indicar a quantidade de números a serem produzidos\n";
        return 1;
    }

    int fd[2];
    pid_t pid;

    if (pipe(fd) == -1) {
        cerr << "Falha ao criar o pipe\n";
        return 1;
    }

    pid = fork();

    if (pid < 0) {
        cerr << "Falha no fork\n";
        return 1;
    }

    const int MSG_SIZE = 20;

    if (pid > 0) { // PRODUTOR
        close(fd[0]);

        long long numero_produzido = 1;
        int incremento_aleatorio;
        int quantidade_a_ser_produzida;

        try {
            quantidade_a_ser_produzida = stoi(argv[1]);
        } catch (...) {
            cerr << "Erro no argumento\n";
            return 1;
        }

        random_device semente;
        mt19937 gerador(semente());
        uniform_int_distribution<> distribuicao(1, 100);

        char buffer[MSG_SIZE];

        while(quantidade_a_ser_produzida--) {
            incremento_aleatorio = distribuicao(gerador);
            numero_produzido += incremento_aleatorio;

            snprintf(buffer, MSG_SIZE, "%lld", numero_produzido);
            write(fd[1], buffer, MSG_SIZE);
        }

        // envia 0 para encerrar
        snprintf(buffer, MSG_SIZE, "0");
        write(fd[1], buffer, MSG_SIZE);

        close(fd[1]);
    } 
    else { // CONSUMIDOR
        close(fd[1]);

        char buffer[MSG_SIZE];
        long long numero_recebido;
        bool primo;

        while(true){
            ssize_t bytes_lidos = read(fd[0], buffer, MSG_SIZE);

            if(bytes_lidos <= 0)
                break;

            numero_recebido = atoll(buffer);
            primo = true;

            if(numero_recebido == 0)
                break;

            for(long long i = 2; i <= sqrt(numero_recebido); i++){
                if(numero_recebido % i == 0){
                    primo = false;
                    break;
                }
            }

            if(primo)
                cout << "O número " << numero_recebido << " é primo\n";
            else
                cout << "O número " << numero_recebido << " NÃO é primo\n";
        }

        close(fd[0]);
    }

    return 0;
}