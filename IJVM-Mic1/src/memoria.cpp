#include "memoria.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>

using namespace std;

// Função para acessar o arquivo de dados, ler todas as linhas, pegar os valores numéricos e salvar em um vetor interno
void Memoria::read()
{
    dados.clear();
    ifstream arq(nomeArquivo);
    if (!arq)
    {
        cerr << "Erro ao abrir o arquivo de dados: " << nomeArquivo << endl;
        dados.resize(8, 0);
        return;
    }
    string linha;
    while (getline(arq, linha))
    {
        istringstream iss(linha);
        int32_t valor;
        iss >> valor;
        dados.push_back(valor);
    }
    while (dados.size() < 8) // completar o arquivo caso necessário
        dados.push_back(0);
    arq.close();
}

Memoria::Memoria(const std::string &nomeArquivo) : nomeArquivo(nomeArquivo)
{
    read();
}

// Função para gravar o estado completo da memória
void Memoria::write() const
{
    ofstream fout(nomeArquivo);
    if (!fout)
    {
        cerr << "Erro ao escrever no arquivo de dados: " << nomeArquivo << endl;
        return;
    }
    for (auto valor : dados)
    {
        fout << valor << endl;
    }
    fout.close();
}

void Memoria::dataWrite(int endereco, int32_t valor)
{
    if (endereco < 0 || endereco >= static_cast<int>(dados.size()))
    {
        cerr << "Endereco invalido " << endl;
        return;
    }
    dados[endereco] = valor;
}

int32_t Memoria::dataRead(int endereco) const
{
    if (endereco < 0 || endereco >= static_cast<int>(dados.size()))
    {
        cerr << "Endereco invalido" << endl;
        return 0;
    }
    return dados[endereco];
}

void Memoria::imprimirDados(ofstream &out) const
{
    for (size_t i = 0; i < dados.size(); ++i)
    {
        // Imprime o conteúdo em binário de 32 bits
        bitset<32> binario(dados[i]);
        out << binario << "\n";
    }
}