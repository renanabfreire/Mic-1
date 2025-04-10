#include "ijvm.h"

void Instrucao(string instrucao, Registradores &regs, Memoria &mem, ofstream &log, int &ciclo, int instrucao_num){
    if(instrucao.find("ILOAD") != string::npos){
        traduzMicroinstucao("H = LV", regs, mem, log, ciclo, instrucao_num);
        ciclo++;

        int num = stoi(instrucao.substr(6));

        for(int i=0; i<num; i++){
            traduzMicroinstucao("H = H+1", regs, mem, log, ciclo, instrucao_num);
            ciclo++;
        }

        traduzMicroinstucao("MAR = H; rd", regs, mem, log, ciclo, instrucao_num);
        ciclo++;

        traduzMicroinstucao("MAR = SP = SP+1; wr", regs, mem, log, ciclo, instrucao_num);
        ciclo++;

        traduzMicroinstucao("TOS = MDR", regs, mem, log, ciclo, instrucao_num);
        ciclo++;
    }

    if(instrucao == "DUP"){
        traduzMicroinstucao("MAR = SP = SP+1", regs, mem, log, ciclo, instrucao_num);
        ciclo++;

        traduzMicroinstucao("MDR = TOS; wr", regs, mem, log, ciclo, instrucao_num);
        ciclo++;
    }

    if(instrucao.find("BIPUSH") != string::npos){
        traduzMicroinstucao("SP = MAR = SP+1", regs, mem, log, ciclo, instrucao_num);
        ciclo++;

        traduzMicroinstucao("fetch" + instrucao.substr(7), regs, mem, log, ciclo, instrucao_num);
        ciclo++;

        traduzMicroinstucao("MDR = TOS = H; wr", regs, mem, log, ciclo, instrucao_num);
        ciclo++;
    }
}

void traduzMicroinstucao(string instrucao, Registradores &regs, Memoria &mem, ofstream &log, int ciclo, int instrucao_num)
{
    string code;

    if(instrucao == "MAR = H; rd") code = "00111000000000001010000";
    else if(instrucao == "H = LV") code = "00110100100000000000101";
    else if(instrucao == "H = H+1") code = "00111001100000000000000";
    else if(instrucao == "MAR = SP = SP+1") code = "00110101000001001000100";
    else if(instrucao == "MAR = SP = SP+1; wr") code = "00110101000001001100100";
    else if(instrucao == "TOS = MDR") code = "00110100001000000000000";
    else if(instrucao == "MDR = TOS; wr") code = "00110100000000010100111";
    else if(instrucao == "MDR = TOS = H; wr") code = "00001000001000010100000";
    else if(instrucao == "SP = MAR = SP+1") code = "00110101000001001000100";
    else if(instrucao == "MDR = TOS = H; wr") code = "00001000001000010101000";
    else if(instrucao.find("fetch") != string::npos) code = instrucao.substr(5) + "000000000110000";

    executarMicroInstrucao(code, regs, mem, log, ciclo, instrucao_num);
}

void executarMicroInstrucao(string instrucao, Registradores &regs, Memoria &mem, ofstream &log, int ciclo, int instrucao_num) {
    if (instrucao.size() != 23) {
        log << "Erro: Instrução inválida com tamanho " << instrucao.size() << " bits (esperado 23)\n";
    }
    log << "Cycle " << ciclo << "\n";
    log << "Instruction " << instrucao_num << "\n";

    string ula = instrucao.substr(0, 8);
    string barraC = instrucao.substr(8, 9);
    string ctrlMem = instrucao.substr(17, 2);
    string barraB = instrucao.substr(19, 4);

    string ir_str = ula + " " + barraC + " " + ctrlMem + " " + barraB;
    log << "ir = " << ir_str << "\n";

    int32_t valor_a = regs.getH();
    int32_t valor_b = 0;
    string nome_b = "Nenhum";

    if (ctrlMem != "11") { // Ignorar barramento B no caso fetch
        auto [vb, nb] = regs.decodificarBarramentoB(barraB);
        valor_b = vb;
        nome_b = nb;
    }

    log << "b = " << nome_b << "\n";

    log << "c = ";
    bool primeiro = true;
    // Corrigido: Ordem dos registradores reflete os bits de barraC (bit 0 = h, bit 8 = mar)
    const vector<string> nomes = {"h", "opc", "tos", "cpp", "lv", "sp", "pc", "mdr", "mar"};
    for (int i = 0; i < 9; ++i) {
        if (barraC[i] == '1') {
            if (!primeiro) log << ", ";
            log << nomes[i];
            primeiro = false;
        }
    }
    log << "\n\n";

    log << "> Registers before instruction\n";
    log << "*******************************\n";
    regs.imprimirEstado(log);

    char ula_bits[8];
    for (int i = 0; i < 8; ++i) ula_bits[i] = ula[i];
    auto [resultado_ula, resultado_sd, N, Z, carry] = ula8bits(ula_bits, valor_a, valor_b);

    regs.seletorBarramentoC(barraC, resultado_sd);

    if (ctrlMem == "10") { // WRITE
        int endereco = regs.getMAR();
        int32_t valor = regs.getMDR();
        mem.dataWrite(endereco, valor);
        mem.write();
    } else if (ctrlMem == "01") { // READ
        int endereco = regs.getMAR();
        int32_t valor = mem.dataRead(endereco);
        regs.setMDR(valor);
    } else if (ctrlMem == "11") { // Caso especial: fetch
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
