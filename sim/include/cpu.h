#ifndef CPU_H_
#define CPU_H_

#include <cstdint>
#include <string>

#include "memory.h"
#include "register-bank.h"
#include "opcode-table.h"
#include "register.h"
#include "opcode.h"
#include "csr.h"

class CPU
{
    MaskedRegister pc_reg;
public:
    CPU(std::function<void(uint32_t,uint32_t,const std::string& s)> on_opcode_decode_, std::function<void(const std::string& s)> on_info_);

    OpCodeTable opcode_table;
    RegisterBank reg;
    Memory memory;
    CSR csr;

    Register& pc = pc_reg;
    bool sleeping = false;

    std::shared_ptr<OpCode> opcode;

    void fetch();
    void exec();

    void step();

    std::function<void(uint32_t,uint32_t,const std::string& s)> on_opcode_decode;

    static constexpr double freq = 1000000.0;
};

#endif
