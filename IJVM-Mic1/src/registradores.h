#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <cstdlib>
#include <utility>

using namespace std;

class Registradores
{
public:
  int32_t H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR;
  uint8_t MBR;
  string IR;

  Registradores();
  /*   Um decodificador de 4 bits que habilita um de 9 registradores a comandar o barramento
B da entrada B da ULA. */
  std::pair<int32_t, std::string> decodificarBarramentoB(const string &bits);
  /*   Um seletor de 9 bits que habilita um ou mais de 9 registradores acima a serem escritos
com o valor na saída da ULA. */
  void seletorBarramentoC(const std::string &bits, int32_t valor);
  void imprimirEstado(const std::string &titulo) const;
  void executarInstrucao(const std::string &instrucao);
};
