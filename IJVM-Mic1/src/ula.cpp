#include "ijvm.h"
#include <cstdint>

std::pair<unsigned int, unsigned int> ula6bits(char input[6])
{
    unsigned int carry_out = 0, output = 0;
    int A = input[2] - 48, B = input[3] - 48;

    if (input[4] == '1')
    {
        if (A == 1)
            A = 0;
        else
            A = 1;
    }

    if (input[0] == '0' && input[1] == '0')
    {
        if (A == 1 && B == 1)
            output = 1;
    }
    else if (input[0] == '0' && input[1] == '1')
    {
        if (A == 1 || B == 1)
            output = 1;
    }
    else if (input[0] == '1' && input[1] == '0')
    {
        if (B == 0)
            output = 1;
    }
    else
    {
        char carry_in = input[5] - 48;

        int sum = A + B + carry_in;

        if (sum == 3)
            return {1, 1};
        else if (sum == 2)
            return {0, 1};
        else if (sum == 1)
            return {1, 0};
    }

    return {output, carry_out};
}

std::tuple<int32_t, int32_t, bool, bool, bool> ula8bits(char input[8], int32_t a, int32_t b)
{ //Saída da ULA = (saída, saída deslocada, N, Z, carry out)
    bool N, Z, carry = false;
    bool sll8 = input[0] == '1';
    bool sra1 = input[1] == '1';
    bool f0 = input[2] == '1';
    bool f1 = input[3] == '1';
    bool ena = input[4] == '1';
    bool enb = input[5] == '1';
    bool inva = input[6] == '1';
    bool inc = input[7] == '1';

    int32_t input_a = ena ? (inva ? ~a : a) : 0;
    int32_t input_b = enb ? b : 0;
    int32_t output, output_sd;

    if (f0 && f1)
    { // Adição
        output = input_a + input_b + (inc ? 1 : 0);
        carry = ((static_cast<uint64_t>(input_a) + static_cast<uint64_t>(input_b) + (inc ? 1 : 0)) >> 32) & 1;
    }
    else if (!f0 && f1)
    { // OR
        output = input_a | input_b;
        carry = false;
    }
    else if (f0 && !f1)
    { // AND
        output = input_a & input_b;
        carry = false;
    }
    else
    { // XOR
        output = input_a ^ input_b;
        carry = false;
    }

    output_sd = output;

    if (sll8)
        output_sd <<= 8;
    if (sra1)
        output_sd >>= 1;

    N = output_sd < 0;
    Z = output_sd == 0;

    return {output, output_sd, N, Z, carry};
}