#ifndef REGISTER_BANK_H_
#define REGISTER_BANK_H_

#include <cstdint>
#include <vector>
#include <array>
#include <iostream>
#include <functional>

#include "register.h"

class RegisterBank
{
public:
    const static unsigned nregs = 16;
private:
    ConstRegister zero;
    std::array<ValueRegister,nregs-1> reg;

    static void on_change_dummy(unsigned, uint32_t) {}
public:
    RegisterBank() : zero(0), reg{}, on_change(on_change_dummy) { };
    std::function<void(unsigned,uint32_t)> on_change;

    Register& operator [](unsigned n) {
        n %= nregs;
        if(n == 0) return zero;
        return reg[n-1];
    }

};

#endif
