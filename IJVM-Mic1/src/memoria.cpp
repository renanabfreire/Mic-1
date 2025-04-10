#include "memoria.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <bitset>

using namespace std;

// Função para acessar o arquivo de dados, ler todas as linhas, pegar os valores numéricos e salvar em um vetor interno
void Memoria::read() {

    dados.clear();
    ifstream arq(nomeArquivo);

    if (!arq) {
        std::cerr << "Erro ao abrir o arquivo de dados: " << nomeArquivo << std::endl;
        dados.resize(8, 0); // Tamanho mínimo de 8, preenchido com 0
        return;
    }

    std::string linha;
    while (std::getline(arq, linha)) {
        // Verifica se a linha tem 32 caracteres (um número binário de 32 bits)
        if (linha.size() == 32 && linha.find_first_not_of("01") == std::string::npos) {
            // Converte a string binária para int32_t usando std::bitset
            std::bitset<32> bits(linha);
            int32_t valor = static_cast<int32_t>(bits.to_ulong());
            dados.push_back(valor);
        } else {
            std::cerr << "Linha inválida no arquivo: " << linha << std::endl;
        }
    }

    // Completa com zeros se menos de 8 elementos
    while (dados.size() < 8) {
        dados.push_back(0);
    }

    arq.close();
}

Memoria::Memoria(const std::string &nomeArquivo) : nomeArquivo(nomeArquivo)
{
    read();
}

// Função para gravar o estado completo da memória
void Memoria::write() const {
    std::ofstream fout(nomeArquivo);
    if (!fout) {
        std::cerr << "Erro ao escrever no arquivo de dados: " << nomeArquivo << std::endl;
        return;
    }

    for (auto valor : dados) {
        // Converte o valor para binário de 32 bits e escreve no arquivo
        std::bitset<32> bits(valor);
        fout << bits.to_string() << std::endl;
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
        //cout << binario << endl;
        out << binario << "\n";
    }
}