#include "ijvm.h"
#include "registradores.h"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include "memoria.h"

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

    ofstream arqs("programa_etapa2_tarefa2_saida.txt");
    if (!arqs)
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
    // r.imprimirEstado();
    cout << "\n=====================================================" << endl;
    cout << "Start of program" << endl;
    cout << "=====================================================" << endl;

    for (size_t ciclo = 0; ciclo < instrucoes.size(); ++ciclo)
    {
        cout << "Cycle " << (ciclo + 1) << endl;
        r.executarInstrucao(instrucoes[ciclo], arqs);
        cout << "=====================================================" << endl;
    }

    cout << "Cycle " << instrucoes.size() << endl;
    cout << "No more lines, EOP." << endl;
    return 1;
}

void executarMicroInstrucao(string instrucao, Registradores &regs, Memoria &mem, ofstream &log, int ciclo)
{
    if (instrucao.size() != 23)
    {
        return;
    }

    log << "Cycle " << ciclo << "\n";

    // Divisão dos campos:
    // uma palavra de bit correspondente a uma microinstru¸c˜ao passar´a a ter 23 bits, da forma:
    // ULA = 8 bits, Barramento C = 9 bits, Memória = 2 bits, Barramento B = 4 bits
    // ULA: bits [0,7] ; Barramento C: bits [8,16] ; Controle de Memória: bits [17,18] ; Barramento B: bits [19,22]
    string ula = instrucao.substr(0, 8);
    string barraC = instrucao.substr(8, 9);
    string ctrlMem = instrucao.substr(17, 2);
    string barraB = instrucao.substr(19, 4);

    string ir_str = ula + " " + barraC + " " + ctrlMem + " " + barraB;
    log << "ir = " << ir_str << "\n";

    auto [valor_b, nome_b] = regs.decodificarBarramentoB(barraB);
    int32_t valor_a = regs.getH();

    log << "b = " << nome_b << "\n";

    log << "c = ";
    bool primeiro = true;
    const vector<string> nomes = {"mar", "mdr", "pc", "sp", "lv", "cpp", "tos", "opc", "h"};
    for (int i = 0; i < 9; ++i)
    {
        if (barraC[i] == '1')
        {
            if (!primeiro)
                log << ", ";
            log << nomes[i];
            primeiro = false;
        }
    }
    log << "\n\n";

    log << "> Registers before instruction\n";
    log << "*******************************\n";
    regs.imprimirEstado(log);

    char ula_bits[8];
    for (int i = 0; i < 8; ++i)
        ula_bits[i] = ula[i];
    auto [resultado_ula, carry] = ula8bits(ula_bits, valor_a, valor_b);

    regs.seletorBarramentoC(barraC, resultado_ula);

    if (ctrlMem == "10")
    {
        int endereco = regs.getMAR();
        int32_t valor = regs.getMDR();
        mem.dataWrite(endereco, valor);
        mem.write();
    }
    else if (ctrlMem == "01")
    {
        int endereco = regs.getMAR();
        int32_t valor = mem.dataRead(endereco);
        regs.setMDR(valor);
    }

    log << "\n> Registers after instruction\n";
    log << "*******************************\n";
    regs.imprimirEstado(log);

    log << "\n> Memory after instruction\n";
    log << "*******************************\n";
    mem.imprimirDados(log);

    log << "============================================================\n";
}

bool etapa3()
{
    Registradores regs;
    Memoria mem("dados_etapa3_tarefa1.txt");

    ifstream microFile("microinstruções_etapa3_tarefa1.txt");
    vector<string> microinstrucoes;
    string linha;

    while (getline(microFile, linha))
    {
        if (!linha.empty())
            microinstrucoes.push_back(linha);
    }

    // Carregar registradores a partir de arquivo
    regs.carregarRegistradores("registradores_etapa3_tarefa1.txt");

    // Arquivo de log
    ofstream log("saída_etapa3_tarefa1.txt");

    // Cabeçalho inicial
    log << "============================================================\n";
    log << "Initial memory state\n";
    log << "*******************************\n";
    mem.imprimirDados(log);
    log << "*******************************\n";
    log << "Initial register state\n";
    log << "*******************************\n";
    regs.imprimirEstado(log);
    log << "============================================================\n";
    log << "Start of Program\n";
    log << "============================================================\n";

    // Execução das microinstruções
    int ciclo = 1;
    for (const auto &instr : microinstrucoes)
    {
        executarMicroInstrucao(instr, regs, mem, log, ciclo);
        ciclo++;
    }

    // Final
    log << "Cycle " << ciclo << "\n";
    log << "No more lines, EOP.\n";

    log.close();
    return 0;
}
int main()
{

    bool etapa;

    etapa = etapa3();
    if (etapa == 1)
        return 0;
    else if (etapa == 0)
        return 1;
}