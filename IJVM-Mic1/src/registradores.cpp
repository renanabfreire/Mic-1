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

        valor = static_cast<uint8_t>(MBR); // zero-extend até 32 bits
        nome = "MBR";
        break;
    }
    case 3: // MBRU com extensão por zeros (unsigned)
    {
        /* Quando MBRU é selecionado, a palavra deve ser preenchida até 32 bits utilizando zeros. */
        valor = static_cast<uint8_t>(MBR); // zero-extend até 32 bits
        nome = "MBRU";
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
    // Verifica se a string possui exatamente 9 bits
    if (bits.size() != 9)
    {
        std::cerr << "Erro: a entrada do seletor deve conter 9 bits." << std::endl;
        return;
    }

    // A convenção é:
    // bits[0] -> H   (bit 8)
    // bits[1] -> OPC (bit 7)
    // bits[2] -> TOS (bit 6)
    // bits[3] -> CPP (bit 5)
    // bits[4] -> LV  (bit 4)
    // bits[5] -> SP  (bit 3)
    // bits[6] -> PC  (bit 2)
    // bits[7] -> MDR (bit 1)
    // bits[8] -> MAR (bit 0)

    if (bits[0] == '1')
        H = valor;
    if (bits[1] == '1')
        OPC = valor;
    if (bits[2] == '1')
        TOS = valor;
    if (bits[3] == '1')
        CPP = valor;
    if (bits[4] == '1')
        LV = valor;
    if (bits[5] == '1')
        SP = valor;
    if (bits[6] == '1')
        PC = valor;
    if (bits[7] == '1')
        MDR = valor;
    if (bits[8] == '1')
        MAR = valor;
}

void Registradores::imprimirEstado(std::ofstream &out) const
{
    // Função lambda para converter um int32_t em uma string de 32 bits
    auto bin = [](int32_t val) -> std::string
    {
        return std::bitset<32>(val).to_string();
    };

    out << "MAR = " << bin(MAR) << "\n";
    out << "MDR = " << bin(MDR) << "\n";
    out << "PC  = " << bin(PC) << "\n";
    // O MBR é de 8 bits; use std::bitset<8>
    out << "MBR = " << std::bitset<8>(static_cast<uint8_t>(MBR)) << "\n";
    out << "SP  = " << bin(SP) << "\n";
    out << "LV  = " << bin(LV) << "\n";
    out << "CPP = " << bin(CPP) << "\n";
    out << "TOS = " << bin(TOS) << "\n";
    out << "OPC = " << bin(OPC) << "\n";
    out << "H   = " << bin(H) << "\n";
}

void Registradores::executarInstrucao(const std::string &instrucao, ofstream &out)
{

    if (instrucao.size() != 21)
    {
        std::cerr << "Erro: instrução deve conter exatamente 21 bits." << std::endl;
        return;
    }

    /* c´odigo deve ser capaz de receber palavras de 21 bits como
    instru¸c˜oes, com o seguinte arranjo: Controle da ULA = 8 bits, Controle do barramento C= 9 bits, Controle do barramento B= 4 bits */

    // Divisão dos campos da instrução:
    // - Controle da ULA: 8 bits (posição 0 a 7)
    // - Controle do barramento C: 9 bits (posição 8 a 16)
    // - Controle do barramento B: 4 bits (posição 17 a 20)
    std::string ulaControl = instrucao.substr(0, 8);
    std::string barramentoC = instrucao.substr(8, 9);
    std::string barramentoB = instrucao.substr(17, 4);

    auto [valorB, nomeB] = decodificarBarramentoB(barramentoB);
    int32_t valorA = H;

    // Imprime nome do barramento B
    out << "b_bus = " << nomeB << std::endl;

    // Imprime nome(s) do(s) registrador(es) escritos no barramento C
    out << "c_bus = ";
    const std::vector<std::string> nomesRegistradores = {"H", "OPC", "TOS", "CPP", "LV", "SP", "PC", "MDR", "MAR"};
    bool primeiro = true;
    for (size_t i = 0; i < barramentoC.size(); ++i)
    {
        if (barramentoC[i] == '1')
        {
            if (!primeiro)
                out << ", ";
            out << nomesRegistradores[i];
            primeiro = false;
        }
    }
    out << "\n\n";

    out << "> Registers before instruction\n";
    imprimirEstado(out);
    out << "\n";

    // Prepara bits da ULA
    char ula_bits[8];
    for (int i = 0; i < 8; ++i)
        ula_bits[i] = ulaControl[i];

    auto [saidaULA, saida_deslocada, N, Z, carry] = ula8bits(ula_bits, valorA, valorB);

    int32_t saida8bit = saidaULA & 0xFF;

    // Atualiza registradores conforme barramento C
    seletorBarramentoC(barramentoC, saidaULA);

    // Estado após a instrução
    out << "> Registers after instruction\n";
    imprimirEstado(out);
    out << "\n";
    IR = instrucao;
}

void Registradores::carregarRegistradores(const string &arquivo)
{
    std::ifstream arq(arquivo);
    if (!arq.is_open())
    {
        std::cerr << "Erro ao abrir arquivo de registradores: " << arquivo << std::endl;
        return;
    }

    std::string linha;
    while (std::getline(arq, linha))
    {
        if (linha.find("mar") != std::string::npos)
            MAR = static_cast<int32_t>(std::bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("mdr") != std::string::npos)
            MDR = static_cast<int32_t>(std::bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("pc") != std::string::npos)
            PC = static_cast<int32_t>(std::bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("mbr") != std::string::npos)
            MBR = static_cast<int8_t>(std::bitset<8>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("sp") != std::string::npos)
            SP = static_cast<int32_t>(std::bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("lv") != std::string::npos)
            LV = static_cast<int32_t>(std::bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("cpp") != std::string::npos)
            CPP = static_cast<int32_t>(std::bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("tos") != std::string::npos)
            TOS = static_cast<int32_t>(std::bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("opc") != std::string::npos)
            OPC = static_cast<int32_t>(std::bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
        else if (linha.find("h") != std::string::npos)
            H = static_cast<int32_t>(std::bitset<32>(linha.substr(linha.find("=") + 2)).to_ulong());
    }
    arq.close();
}