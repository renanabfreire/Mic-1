
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

using namespace std;

class Memoria
{
private:
    // Arquivo de memória
    vector<int32_t> dados;
    string nomeArquivo;

public:
    Memoria(const string &nomeArquivo);

    void read();

    void write() const;

    void dataWrite(int endereco, int32_t valor);

    int32_t dataRead(int endereco) const;

    void imprimirDados(ofstream &out) const;
};
