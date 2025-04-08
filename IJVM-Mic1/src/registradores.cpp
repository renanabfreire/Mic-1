#include "registradores.h"
#include "ijvm.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <bitset>

Registradores::Registradores()
{
    H = 0;
    OPC = 0;
    TOS = 0;
    CPP = 0;
    LV = 0;
    SP = 0;
    PC = 0;
    MDR = 0;
    MAR = 0;
    MBR = 0;
}

std::pair<int32_t, std::string> Registradores::decodificarBarramentoB(const std::string &bits)
{
    int sel = std::stoi(bits, nullptr, 2);
    int32_t valor = 0;
    std::string nome;

    /*   Para o caso do decodificador de 4 bits, deve ser seguida a convenção abaixo para a relação
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
    case 2:
    {
        /*  Quando MBR é selecionado, o bit mais alto de MBR, que é o bit de sinal, deve ser utilizado para preencher
        a palavra de 8 bits até que esta tenha os 32 bits necessários */
        int8_t mbr_signed = static_cast<int8_t>(MBR);
        valor = mbr_signed;
        nome = "MBR (signed)";
        break;
    }
    case 3:
    {
        /* Quando MBRU é selecionado, a palavra deve ser preenchida até 32 bits utilizando zeros.  */
        valor = MBR;
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
    /* O seletor segue a seguinte ordem: H=8 OPC=7 TOS=6 CPP=5 LV=4 SP=3 PC=2 MDR=1 MAR=0  */
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

void Registradores::imprimirEstado(const std::string &titulo) const
{
    std::cout << titulo << std::endl;
    std::cout << "H   = " << H << std::endl;
    std::cout << "OPC = " << OPC << std::endl;
    std::cout << "TOS = " << TOS << std::endl;
    std::cout << "CPP = " << CPP << std::endl;
    std::cout << "LV  = " << LV << std::endl;
    std::cout << "SP  = " << SP << std::endl;
    std::cout << "MBR = " << static_cast<int>(MBR) << std::endl;
    std::cout << "PC  = " << PC << std::endl;
    std::cout << "MDR = " << MDR << std::endl;
    std::cout << "MAR = " << MAR << std::endl;
}

void Registradores::executarInstrucao(const std::string &instrucao)
{
    if (instrucao.size() != 21)
    {
        std::cerr << "ERRO" << endl;
        return;
    }

    /* c´odigo deve ser capaz de receber palavras de 21 bits como
instru¸c˜oes, com o seguinte arranjo: Controle da ULA = 8 bits,  Controle do barramento C= 9 bits, Controle do barramento B= 4 bits */

    string ula = instrucao.substr(0, 8);
    string barraC = instrucao.substr(8, 9);
    string barraB = instrucao.substr(17, 4);

    auto [valor_b, nome_b] = decodificarBarramentoB(barraB);
    int32_t valor_a = H;

    char ula_bits[8];
    for (int i = 0; i < 8; ++i)
    {
        ula_bits[i] = ula[i];
    }

    auto [saida_ula, carry] = ula8bits(ula_bits, valor_a, valor_b);
    seletorBarramentoC(barraC, saida_ula);

    IR = std::bitset<21>(instrucao).to_ulong();
}