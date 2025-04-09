#include "ijvm.h"

void Instrucao(string instrucao, Registradores &regs, Memoria &mem, ofstream &log, int ciclo, int& PC){
    if(instrucao.find("ILOAD") != string::npos){
        traduzMicroinstucao("H = LV", regs, mem, log, ciclo);
        PC++;

        int num = stoi(instrucao.substr(6));

        for(int i=0; i<num; i++){
            traduzMicroinstucao("H = H+1", regs, mem, log, ciclo);
            PC++;
        }

        traduzMicroinstucao("MAR = H; rd", regs, mem, log, ciclo);
        PC++;

        traduzMicroinstucao("MAR = SP = SP+1; wr", regs, mem, log, ciclo);
        PC++;

        traduzMicroinstucao("TOS = MDR", regs, mem, log, ciclo);
        PC++;
    }

    if(instrucao == "DUP"){
        traduzMicroinstucao("MAR = SP = SP+1", regs, mem, log, ciclo);
        PC++;

        traduzMicroinstucao("MDR = TOS; wr", regs, mem, log, ciclo);
        PC++;
    }
}

void traduzMicroinstucao(string instrucao, Registradores &regs, Memoria &mem, ofstream &log, int ciclo){
    string code;

    if(instrucao == "MAR = H; rd") code = "00111000100000000010000";
    else if(instrucao == "H = LV") code = "00111000100000000100011";
    else if(instrucao == "H = H+1") code = "00110101100000000101000";
    else if(instrucao == "MAR = SP = SP+1") code = "00110101000001001010100";
    else if(instrucao == "MAR = SP = SP+1; wr") code = "00110101000001001100100";
    else if(instrucao == "TOS = MDR") code = "00110100101000000100000";
    else if(instrucao == "MDR = TOS; wr") code = "00110100000000010100111";

    executarMicroInstrucao(code, regs, mem, log, ciclo);
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
    auto [resultado_ula, resultado_sd, N, Z, carry] = ula8bits(ula_bits, valor_a, valor_b);

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
