#include "registradores.h"
#include "ijvm.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <bitset>
#include <vector>

Registradores::Registradores()
{
    H = 1; // Como no seu exemplo
    OPC = 0;
    TOS = 2; // Também como no seu exemplo
    CPP = 0;
    LV = 0;
    SP = 0;
    PC = 0;
    MDR = 0;
    MAR = 0;
    MBR = 0b10000001;
}

std::pair<int32_t, std::string> Registradores::decodificarBarramentoB(const std::string &bits)
{
    int sel = std::stoi(bits, nullptr, 2);
    int32_t valor = 0;
    std::string nome;

    /* Para o caso do decodificador de 4 bits, deve ser seguida a convenção abaixo para a relação
    entre os registradores e a saída habilitada no decodificador: */
    switch (sel)
    {
    case 0:
        valor = MDR;
        nome = "MDR";
        break;
    case 1:
        valor = PC;
        nome = "PC";
        break;
    case 2: // MBR com extensão de sinal (signed)
    {
        /* Quando MBR é selecionado, o bit mais alto de MBR, que é o bit de sinal, deve ser utilizado para preencher
        a palavra de 8 bits até que esta tenha os 32 bits necessários */
        int8_t mbr_signed = static_cast<int8_t>(MBR); // preserva o sinal
        valor = static_cast<int32_t>(mbr_signed);     // extensão de sinal para 32 bits
        nome = "MBR (signed)";
        break;
    }
    case 3: // MBRU com extensão por zeros (unsigned)
    {
        /* Quando MBRU é selecionado, a palavra deve ser preenchida até 32 bits utilizando zeros. */
        valor = static_cast<uint8_t>(MBR); // zero-extend até 32 bits
        nome = "MBR (unsigned)";
        break;
    }
    case 4:
        valor = SP;
        nome = "SP";
        break;
    case 5:
        valor = LV;
        nome = "LV";
        break;
    case 6:
        valor = CPP;
        nome = "CPP";
        break;
    case 7:
        valor = TOS;
        nome = "TOS";
        break;
    case 8:
        valor = OPC;
        nome = "OPC";
        break;
    default:
        valor = 0;
        nome = "Desconhecido";
        break;
    }
    return {valor, nome};
}

void Registradores::seletorBarramentoC(const std::string &bits, int32_t valor)
{
    /* O seletor segue a seguinte ordem: H=8 OPC=7 TOS=6 CPP=5 LV=4 SP=3 PC=2 MDR=1 MAR=0 */
    /* Por exemplo, se a entrada do seletor for 100001000, os registradores H (bit 8) e SP (bitn3) estarão habilitados para escrita no barramento C */
    if (bits[0] == '1')
        MAR = valor;
    if (bits[1] == '1')
        MDR = valor;
    if (bits[2] == '1')
        PC = valor;
    if (bits[3] == '1')
        SP = valor;
    if (bits[4] == '1')
        LV = valor;
    if (bits[5] == '1')
        CPP = valor;
    if (bits[6] == '1')
        TOS = valor;
    if (bits[7] == '1')
        OPC = valor;
    if (bits[8] == '1')
        H = valor;
}

void Registradores::imprimirEstado(std::ofstream &out) const
{
    auto bin = [](int32_t val)
    {
        return std::bitset<32>(val).to_string();
    };

    out << "mar = " << bin(MAR) << "\n";
    out << "mdr = " << bin(MDR) << "\n";
    out << "pc = " << bin(PC) << "\n";
    out << "mbr = " << std::bitset<8>(MBR) << "\n";
    out << "sp = " << bin(SP) << "\n";
    out << "lv = " << bin(LV) << "\n";
    out << "cpp = " << bin(CPP) << "\n";
    out << "tos = " << bin(TOS) << "\n";
    out << "opc = " << bin(OPC) << "\n";
    out << "h = " << bin(H) << "\n";
}

void Registradores::executarInstrucao(const std::string &instrucao, ofstream &out)
{
    if (instrucao.size() != 21)
    {
        std::cerr << "ERRO" << endl;
        return;
    }

    /* c´odigo deve ser capaz de receber palavras de 21 bits como
    instru¸c˜oes, com o seguinte arranjo: Controle da ULA = 8 bits, Controle do barramento C= 9 bits, Controle do barramento B= 4 bits */

    string ula = instrucao.substr(0, 8);
    string barraC = instrucao.substr(8, 9);
    string barraB = instrucao.substr(17, 4);

    auto [valor_b, nome_b] = decodificarBarramentoB(barraB);
    int32_t valor_a = H;

    std::cout << "b_bus = " << nome_b << std::endl;

    // Lista registradores escritos
    std::cout << "c_bus = ";
    bool primeiro = true;
    const std::vector<std::string> nomes = {"mar", "mdr", "pc", "sp", "lv", "cpp", "tos", "opc", "h"};
    for (int i = 0; i < 9; ++i)
    {
        if (barraC[i] == '1')
        {
            if (!primeiro)
                std::cout << ", ";
            std::cout << nomes[i];
            primeiro = false;
        }
    }
    std::cout << std::endl
              << std::endl;

    std::cout << "> Registers before instruction" << std::endl;
    imprimirEstado(out);
    std::cout << std::endl;

    char ula_bits[8];
    for (int i = 0; i < 8; ++i)
    {
        ula_bits[i] = ula[i];
    }

    auto [saida_ula, saida_sd, N, Z, carry] = ula8bits(ula_bits, valor_a, valor_b);
    std::cout << "Resultado da ULA: " << saida_ula << std::endl;
    seletorBarramentoC(barraC, saida_ula);

    std::cout << "> Registers after instruction" << std::endl;
    imprimirEstado(out);
    std::cout << std::endl;

    IR = instrucao;
}

void Registradores::carregarRegistradores(const string &arquivo)
{
    ifstream arq(arquivo);
    if (!arq.is_open())
    {
        cerr << "Erro ao abrir arquivo de registradores: " << arquivo << endl;
        return;
    }

    string linha;
    while (getline(arq, linha))
    {
        if (linha.find("mar") != string::npos)
            MAR = static_cast<int32_t>(bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("mdr") != string::npos)
            MDR = static_cast<int32_t>(bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("pc") != string::npos)
            PC = static_cast<int32_t>(bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("mbr") != string::npos)
            MBR = static_cast<int8_t>(bitset<8>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("sp") != string::npos)
            SP = static_cast<int32_t>(bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("lv") != string::npos)
            LV = static_cast<int32_t>(bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("cpp") != string::npos)
            CPP = static_cast<int32_t>(bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("tos") != string::npos)
            TOS = static_cast<int32_t>(bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("opc") != string::npos)
            OPC = static_cast<int32_t>(bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("h") != string::npos)
            H = static_cast<int32_t>(bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
    }

    arq.close();
}