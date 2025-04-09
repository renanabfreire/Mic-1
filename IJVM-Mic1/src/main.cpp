#include "ijvm.h"
#include "registradores.h"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>

using namespace std;

bool etapa1()
{
    // Leitura de arquivos:
    // Abrir os arquivos
    ifstream entrada("programa_etapa1.txt");
    ofstream saida("saída_etapa1.txt");

    if (!entrada.is_open() || !saida.is_open())
    {
        cerr << "Erro ao abrir arquivos de entrada ou saída.\n";
        return 0;
    }

    // Início do programa
    saida << "b = " << bitset<32>(1) << "\n";
    // Aqui usamos -1 para representar 0xFFFFFFFF (todos 1)
    saida << "a = " << bitset<32>(static_cast<unsigned int>(-1)) << "\n\n";
    saida << "Start of Program\n";
    saida << "============================================================\n";

    string linha;
    int PC = 0;

    /*     Instrução:  Esta palavra deve ser armazenada em uma variável que representa o registrador de
    instrução (IR), e uma outra variável deve atuar como o contador de programa (PC),
    definindo que linha do programa está sendo executada. */
    while (getline(entrada, linha))
    {

        if (linha.empty())
        {
            saida << "Cycle " << (PC + 1) << "\n\n";
            saida << "PC = " << (PC + 1) << "\n";
            saida << "> Line is empty, EOP.\n";
            break;
        }

        if (linha.length() != 6)
        {
            cerr << "Erro\n";
            continue;
        }
        char IR[6];
        copy(linha.begin(), linha.begin() + 6, IR);

        // Chama a função da ULA
        pair<unsigned int, unsigned int> resultado_Ula = ula6bits(IR);
        int A = IR[2] - '0';
        int B = IR[3] - '0';

        int S = resultado_Ula.first;
        int vai_um = resultado_Ula.second;

        saida << "Cycle " << (PC + 1) << "\n\n";
        saida << "PC = " << (PC + 1) << "\n";
        saida << "IR = " << linha << "\n";
        saida << "b = " << bitset<32>(B) << "\n";
        saida << "a = " << bitset<32>(-A) << "\n";
        saida << "s = " << bitset<32>(S) << "\n";
        saida << "co = " << vai_um << "\n";
        saida << "============================================================\n";

        PC++;
    }

    entrada.close();
    saida.close();
    return 1;
}

bool etapa2()
{

    ifstream arq("programa_etapa2_tarefa2.txt");
    if (!arq)
    {
        cerr << "Erro ao abrir o arquivo programa.txt" << endl;
        return 0;
    }

    vector<string> instrucoes;
    string linha;
    while (getline(arq, linha))
    {
        if (!linha.empty())
        {
            cout << linha << endl;
            instrucoes.push_back(linha);
        }
    }

    cout << "=====================================================" << endl;
    cout << "> Initial register states" << endl;
    Registradores r;
    r.imprimirEstado();
    cout << "\n=====================================================" << endl;
    cout << "Start of program" << endl;
    cout << "=====================================================" << endl;

    for (size_t ciclo = 0; ciclo < instrucoes.size(); ++ciclo)
    {
        cout << "Cycle " << (ciclo + 1) << endl;
        r.executarInstrucao(instrucoes[ciclo]);
        cout << "=====================================================" << endl;
    }

    cout << "Cycle " << instrucoes.size() << endl;
    cout << "No more lines, EOP." << endl;
    return 1;
}

int main()
{

    bool etapa;

    etapa = etapa2();
    if (etapa == 1)
        return 0;
    else if (etapa == 0)
        return 1;
}