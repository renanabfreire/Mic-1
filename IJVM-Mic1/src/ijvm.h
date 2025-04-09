#include "registradores.h"
#include "memoria.h"

#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <iostream>
#include <tuple>
using namespace std;

std::pair<unsigned int, unsigned int> ula6bits(char[6]);
std::tuple<int32_t, int32_t, bool, bool, bool> ula8bits(char input[8], int32_t a, int32_t b);

void executarMicroInstrucao(string, Registradores &, Memoria &, ofstream &, int);
