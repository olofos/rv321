#include <iostream>
#include <iomanip>

#include "opcode.h"
#include "cpu.h"


uint32_t OpCode::immI() const
{
    return static_cast<uint32_t>(static_cast<int32_t>(instruction) >> 20);
}

uint32_t OpCode::immS() const
{
    return static_cast<uint32_t>((static_cast<int32_t>(instruction) >> 20) & 0xFFFFFFE0) | ((instruction >> 7) & 0x0000001F);
}

uint32_t OpCode::immB() const
{
    return static_cast<uint32_t>((static_cast<int32_t>(instruction) >> 20) & 0xFFFFF7E0) | ((instruction << 4) & 0x00000800) | ((instruction >> 7) & 0x0000001E);
}

uint32_t OpCode::immU() const
{
    return instruction & 0xFFFFF000;
}

uint32_t OpCode::immJ() const
{
    return static_cast<uint32_t>((static_cast<int32_t>(instruction) >> 20) & 0xFFF007FE) | (instruction & 0x000FF000) | ((instruction >> 9) & 0x00000800);
}



void BinOp::exec(CPU &cpu) const
{
    uint32_t a = cpu.reg[rs1];
    uint32_t b = cpu.reg[rs2];
    uint32_t res = func(a,b);
    cpu.reg[rd] = res;

    cpu.pc += len;
    cpu.csr["mcycle"] += cycles;
}


void BinImmOp::exec(CPU &cpu) const
{
    uint32_t a = cpu.reg[rs1];
    uint32_t b = immI();
    uint32_t res = func(a,b);
    cpu.reg[rd] = res;

    cpu.pc += len;
    cpu.csr["mcycle"] += cycles;
}

void BranchOp::exec(CPU &cpu) const
{
    uint32_t a = cpu.reg[rs1];
    uint32_t b = cpu.reg[rs2];

    if(comp(a,b)) {
        cpu.pc += immB();
        cpu.csr["mcycle"] += cycles + 34;
    } else {
        cpu.pc += len;
        cpu.csr["mcycle"] += cycles;
    }
}

void LUIOp::exec(CPU &cpu) const
{
    uint32_t res = immU();
    cpu.reg[rd] = res;
    cpu.pc += len;
    cpu.csr["mcycle"] += cycles;
}

std::string LUIOp::format(const CPU&) const
{
    std::stringstream ss;
    ss << name << " x" << static_cast<int>(rd) << ", 0x" << std::uppercase << std::hex << ( immU() >> 12 );
    return ss.str();
}

void AUIPCOp::exec(CPU &cpu) const
{
    uint32_t res = immU() + cpu.pc;

    cpu.reg[rd] = res;
    cpu.pc += len;
    cpu.csr["mcycle"] += cycles;
}

void JALOp::exec(CPU &cpu) const
{
    uint32_t link = cpu.pc + len;
    uint32_t dest = cpu.pc + immJ();

    cpu.reg[rd] = link;
    cpu.pc = dest;
    cpu.csr["mcycle"] += cycles;
}

void JALROp::exec(CPU &cpu) const
{
    uint32_t link = cpu.pc + len;
    uint32_t dest = ( cpu.reg[rs1] + immI() ) & ~1;

    cpu.reg[rd] = link;
    cpu.pc = dest;
    cpu.csr["mcycle"] += cycles;
}

void LoadOp::exec(CPU &cpu) const
{
    uint32_t address = cpu.reg[rs1] + immI();
    uint32_t res = read(cpu.memory, address);

    cpu.reg[rd] = res;
    cpu.pc += len;
    cpu.csr["mcycle"] += cycles;
}

void StoreOp::exec(CPU &cpu) const
{
    uint32_t address = cpu.reg[rs1] + immS();
    uint32_t data = cpu.reg[rs2];

    write(cpu.memory, address, data);
    cpu.pc += len;
    cpu.csr["mcycle"] += cycles;
}

void NopOp::exec(CPU &cpu) const
{
    cpu.pc += len;
    cpu.csr["mcycle"] += cycles;
}

std::string CSROp::format(const CPU& cpu) const {
    return name + " x" + std::to_string(rd) + ", " + cpu.csr.get_name(immI() & 0xFFF) + ", x" + std::to_string(rs1);
}

void CSROp::exec(CPU &cpu) const
{
    uint16_t address = immI() & 0xFFF;
    uint32_t old_val = cpu.csr[address];
    uint32_t new_val = op(cpu.reg[rs1], old_val);

    cpu.csr[address] = new_val;
    cpu.reg[rd] = old_val;

    cpu.pc += len;
    cpu.csr["mcycle"] += cycles;
}

void CSRImmOp::exec(CPU &cpu) const
{
    uint16_t address = immI() & 0xFFF;
    uint32_t old_val = cpu.csr[address];
    uint32_t new_val = op(rs1, old_val);

    cpu.csr[address] = new_val;
    cpu.reg[rd] = old_val;

    cpu.pc += len;
    cpu.csr["mcycle"] += cycles;
}

std::string TrapOp::format(const CPU&) const
{
    uint32_t trap2 = immI() & 0x00000003;

    if(trap2 == 0) { // ECALL
        return "ecall";
    } else if(trap2 == 1) { // EBREAK
        return "ebreak";
    } else {
        return name;
    }
}

void TrapOp::exec(CPU &cpu) const
{
    cpu.csr["mepc"] = cpu.pc;
    cpu.pc = cpu.csr["mtvec"];

    uint32_t mstatus = cpu.csr["mstatus"];
    cpu.csr["mstatus"] = (mstatus & 0xFFFFFF88) | ((mstatus & 0x00000008) << 4);

    uint32_t trap2 = immI() & 0x00000003;

    if(trap2 == 0) { // ECALL
        cpu.csr["mcause"] = 0x0000000B;
    } else if(trap2 == 1) { // EBREAK
        cpu.csr["mcause"] = 0x00000003;
    } else { // Illegal instruction or Interrupt
        cpu.csr["mcause"] = 0x02;
    }

    cpu.csr["mcycle"] += cycles;
}

void MRETOp::exec(CPU &cpu) const
{
    cpu.pc = cpu.csr["mepc"];

    uint32_t mstatus = cpu.csr["mstatus"];
    cpu.csr["mstatus"] = (mstatus & 0xFFFFFF88) | ((mstatus & 0x00000080) >> 4) | 0x00000080;

    cpu.csr["mcycle"] += cycles;
}

void WFIOp::exec(CPU &cpu) const
{
    uint32_t mask = cpu.csr["mie"];
    uint32_t irq = cpu.csr["mip"];

    if(irq & mask) {
        cpu.sleeping = false;
        cpu.pc += len;
        cpu.csr["mcycle"] += cycles;
    } else {
        cpu.sleeping = true;
    }
}
