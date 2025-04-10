#include "ijvm.h"

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
    ifstream arq("../data/programa_etapa2_tarefa2.txt");
    ofstream saida("saida_etapa2.txt");

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
            saida << linha << endl;
            instrucoes.push_back(linha);
        }
    }

    saida << "=====================================================" << endl;
    saida << "> Initial register states" << endl;
    Registradores r;
    r.imprimirEstado(saida);
    saida << "\n=====================================================" << endl;
    saida << "Start of program" << endl;
    saida << "=====================================================" << endl;

    for (size_t ciclo = 0; ciclo < instrucoes.size(); ++ciclo)
    {
        saida << "Cycle " << (ciclo + 1) << endl;
        r.executarInstrucao(instrucoes[ciclo], saida);
        saida << "=====================================================" << endl;
    }

    saida << "Cycle " << instrucoes.size() + 1 << endl;
    saida << "No more lines, EOP." << endl;

    return 1;
}

void etapa2_tarefa1()
{

    ifstream arq("../data/programa_etapa2_tarefa1.txt");
    if (!arq)
    {
        cerr << "Erro ao abrir o arquivo programa.txt" << endl;
        return;
    }

    int32_t b = 0b10000000000000000000000000000000;
    int32_t a = 0b00000000000000000000000000000001;

    cout << "b = " << bitset<32>(b) << endl;
    cout << "a = " << bitset<32>(a) << endl;
    cout << "\n=====================================================" << endl;
    cout << "Start of program" << endl;

    vector<string> instrucoes;
    string linha;
    int cont = 1;

    while (getline(arq, linha))
    {
        if (!linha.empty())
        {
            cout << linha << endl;
            instrucoes.push_back(linha);
        }

        char IR[8];
        if (linha.size() >= 8)
        {
            copy(linha.begin(), linha.begin() + 8, IR);
        }
        else
        {
            cerr << "Instrução inválida: " << linha << endl;
            continue;
        }

        auto saida_ULA = ula8bits(IR, a, b);

        cout << "Cycle " << cont << endl
             << endl;
        cout << "PC: " << cont << endl; // PC em decimal, pois é um contador
        cout << "IR: " << IR << endl;
        cout << "b: " << bitset<32>(b) << endl;
        cout << "a: " << bitset<32>(a) << endl;
        cout << "Saida: " << bitset<32>(get<0>(saida_ULA)) << "\n";           // Saída em binário
        cout << "Saida deslocada: " << bitset<32>(get<1>(saida_ULA)) << "\n"; // Saída deslocada em binário
        cout << "N: " << get<2>(saida_ULA) << "\n";                           // Negativo (0 ou 1)
        cout << "Z: " << get<3>(saida_ULA) << "\n";                           // Zero (0 ou 1)
        cout << "Carry: " << get<4>(saida_ULA) << "\n";                       // Carry (0 ou 1)

        cout << "=====================================================" << endl;
        cont++;
    }
}

void executarMicroInstrucao(string instrucao, Registradores &regs, Memoria &mem, ofstream &log, int ciclo)
{
    if (instrucao.size() != 23)
    {
        log << "Erro: Instrução inválida com tamanho " << instrucao.size() << " bits (esperado 23)\n";
        return;
    }

    log << "Cycle " << ciclo << "\n";

    string ula = instrucao.substr(0, 8);
    string barraC = instrucao.substr(8, 9);
    string ctrlMem = instrucao.substr(17, 2);
    string barraB = instrucao.substr(19, 4);

    string ir_str = ula + " " + barraC + " " + ctrlMem + " " + barraB;
    log << "ir = " << ir_str << "\n";

    int32_t valor_a = regs.getH();
    int32_t valor_b = 0;
    string nome_b = "Nenhum";

    if (ctrlMem != "11")
    { // Ignorar barramento B no caso fetch
        auto [vb, nb] = regs.decodificarBarramentoB(barraB);
        valor_b = vb;
        nome_b = nb;
    }

    log << "b = " << nome_b << "\n";

    log << "c = ";
    bool primeiro = true;
    // Corrigido: Ordem dos registradores reflete os bits de barraC (bit 0 = h, bit 8 = mar)
    const vector<string> nomes = {"h", "opc", "tos", "cpp", "lv", "sp", "pc", "mdr", "mar"};
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
    auto [resultado_ula, resultado_sd, N, Z, carry] = ula8bits(ula_bits, valor_a, valor_b);

    regs.seletorBarramentoC(barraC, resultado_sd);

    if (ctrlMem == "10")
    { // WRITE
        int endereco = regs.getMAR();
        int32_t valor = regs.getMDR();
        mem.dataWrite(endereco, valor);
        mem.write();
    }
    else if (ctrlMem == "01")
    { // READ
        int endereco = regs.getMAR();
        int32_t valor = mem.dataRead(endereco);
        regs.setMDR(valor);
    }
    else if (ctrlMem == "11")
    { // Caso especial: fetch
        int8_t valor_mbr = static_cast<int8_t>(stoi(ula, nullptr, 2));
        regs.MBR = valor_mbr;
        regs.H = static_cast<int32_t>(static_cast<uint8_t>(valor_mbr)); // Preenchimento com zeros
    }

    log << "\n> Registers after instruction\n";
    log << "*******************************\n";
    regs.imprimirEstado(log);

    log << "\n> Memory after instruction\n";
    log << "*******************************\n";
    mem.imprimirDados(log);

    log << "============================================================\n";
}

void etapa3()
{
    Registradores regs;
    Memoria mem("../data/dados_etapa3_tarefa1.txt");

    ifstream microFile("../data/microinstrues_etapa3_tarefa1.txt");
    vector<string> microinstrucoes;
    string linha;

    while (getline(microFile, linha))
    {
        if (!linha.empty())
            microinstrucoes.push_back(linha);
    }

    // Carregar registradores a partir de arquivo
    regs.carregarRegistradores("../data/registradores_etapa3_tarefa1.txt");

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

    } // Final
    log << "Cycle " << ciclo << "\n";
    log << "No more lines, EOP.\n";

    log.close();
}

int main()
{
    etapa3();
}