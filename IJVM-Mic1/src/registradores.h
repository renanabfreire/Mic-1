#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <bitset>

using namespace std;

class Registradores
{
public:
  int32_t H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR;
  uint8_t MBR;
  string IR;

  Registradores();
  /* Um decodificador de 4 bits que habilita um de 9 registradores a comandar o barramento
  B da entrada B da ULA. */
  std::pair<int32_t, std::string> decodificarBarramentoB(const std::string &bits);
  /* Um seletor de 9 bits que habilita um ou mais de 9 registradores acima a serem escritos
  com o valor na saída da ULA. */
  void seletorBarramentoC(const std::string &bits, int32_t valor);
  void imprimirEstado(std::ofstream &out) const;
  void executarInstrucao(const std::string &instrucao, ofstream &out);
  void carregarRegistradores(const string &arquivo);

  int32_t getH() const { return H; }
  int32_t getMAR() const { return MAR; }
  int32_t getMDR() const { return MDR; }
  void setMDR(int32_t valor) { MDR = valor; }
};
