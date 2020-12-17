#include <iostream>
#include <iomanip>

#include "cpu.h"
#include "opcode.h"
#include "opcode-table.h"

std::shared_ptr<OpCodeTableEntry> bin_op(std::string name, uint8_t func3, uint8_t sa, uint32_t cycles, std::function<uint32_t(uint32_t,uint32_t)> op)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b01100, func3, sa, 0, [name, cycles, op] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<BinOp>(name, instruction, raw_instruction, cycles, op); } );
}

std::shared_ptr<OpCodeTableEntry> bin_imm_op(std::string name, uint8_t func3, uint32_t cycles, std::function<uint32_t(uint32_t,uint32_t)> op)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b00100, func3, OpCodeTable::any, OpCodeTable::any, [name, cycles, op] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<BinImmOp>(name, instruction, raw_instruction, cycles, op); } );
}

std::shared_ptr<OpCodeTableEntry> shift_imm_op(std::string name, uint8_t func3, uint8_t sa, uint32_t cycles, std::function<uint32_t(uint32_t,uint32_t)> op)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b00100, func3, sa, OpCodeTable::any, [name, cycles, op] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<BinImmOp>(name, instruction, raw_instruction, cycles, op); } );
}

std::shared_ptr<OpCodeTableEntry> branch_op(std::string name, uint8_t func3, uint32_t cycles, std::function<bool(uint32_t,uint32_t)> comp)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b11000, func3, OpCodeTable::any, OpCodeTable::any, [name, cycles, comp] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<BranchOp>(name, instruction, raw_instruction, cycles, comp); } );
}

std::shared_ptr<OpCodeTableEntry> lui_op(std::string name, uint32_t cycles)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b01101, OpCodeTable::any, OpCodeTable::any, OpCodeTable::any, [name, cycles] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<LUIOp>(name, instruction, raw_instruction, cycles); } );
}

std::shared_ptr<OpCodeTableEntry> auipc_op(std::string name, uint32_t cycles)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b00101, OpCodeTable::any, OpCodeTable::any, OpCodeTable::any, [name, cycles] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<AUIPCOp>(name, instruction, raw_instruction, cycles); } );
}

std::shared_ptr<OpCodeTableEntry> jal_op(std::string name, uint32_t cycles)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b11011, OpCodeTable::any, OpCodeTable::any, OpCodeTable::any, [name, cycles] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<JALOp>(name, instruction, raw_instruction, cycles); } );
}

std::shared_ptr<OpCodeTableEntry> jalr_op(std::string name, uint32_t cycles)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b11001, 0b000, OpCodeTable::any, OpCodeTable::any, [name, cycles] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<JALROp>(name, instruction, raw_instruction, cycles); } );
}

std::shared_ptr<OpCodeTableEntry> load_op(std::string name, uint8_t func3, uint32_t cycles, std::function<uint32_t(Memory&,uint32_t)> read)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b00000, func3, OpCodeTable::any, OpCodeTable::any, [name, cycles, read] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<LoadOp>(name, instruction, raw_instruction, cycles, read); } );
}

std::shared_ptr<OpCodeTableEntry> store_op(std::string name, uint8_t func3, uint32_t cycles, std::function<void(Memory&,uint32_t,uint32_t)> write)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b01000, func3, OpCodeTable::any, OpCodeTable::any, [name, cycles, write] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<StoreOp>(name, instruction, raw_instruction, cycles, write); } );
}

std::shared_ptr<OpCodeTableEntry> nop_op(std::string name, uint8_t opclass, uint8_t func3, uint8_t sa, uint8_t sys, uint32_t cycles)
{
    return std::make_shared<OpCodeTableEntry>(name, opclass, func3, sa, sys, [name, cycles] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<NopOp>(name, instruction, raw_instruction, cycles); } );
}

std::shared_ptr<OpCodeTableEntry> csr_op(std::string name, uint8_t func3, uint32_t cycles, std::function<uint32_t(uint32_t,uint32_t)> op)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b11100, func3, OpCodeTable::any, OpCodeTable::any, [name, cycles, op] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<CSROp>(name, instruction, raw_instruction, cycles, op); } );
}

std::shared_ptr<OpCodeTableEntry> csr_imm_op(std::string name, uint8_t func3, uint32_t cycles, std::function<uint32_t(uint32_t,uint32_t)> op)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b11100, func3, OpCodeTable::any, OpCodeTable::any, [name, cycles, op] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<CSRImmOp>(name, instruction, raw_instruction, cycles, op); } );
}

std::shared_ptr<OpCodeTableEntry> trap_op(std::string name, uint32_t cycles)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b11100, 0b000, 0b0, 0b0, [name, cycles] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<TrapOp>(name, instruction, raw_instruction, cycles); } );
}

std::shared_ptr<OpCodeTableEntry> mret_op(std::string name, uint32_t cycles)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b11100, 0b000, 0b1, 0b1, [name, cycles] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<MRETOp>(name, instruction, raw_instruction, cycles); } );
}

std::shared_ptr<OpCodeTableEntry> wfi_op(std::string name, uint32_t cycles)
{
    return std::make_shared<OpCodeTableEntry>(name, 0b11100, 0b000, 0b0, 0b1, [name, cycles] (uint32_t instruction, uint32_t raw_instruction) { return std::make_shared<WFIOp>(name, instruction, raw_instruction, cycles); } );
}


CPU::CPU(std::function<void(uint32_t,uint32_t,const std::string& s)> on_opcode_decode_, std::function<void(const std::string& s)> on_info_) :
    pc_reg(0xFFFFFFFE),
    memory(on_info_),
    csr(on_info_),
    pc{pc_reg},
    sleeping(false),
    on_opcode_decode(on_opcode_decode_)
{
    opcode_table = {
        bin_op("add",  0b000, 0, 105, [] (uint32_t a, uint32_t b) { return a + b; }),
        bin_op("sub",  0b000, 1, 105, [] (uint32_t a, uint32_t b) { return a - b; }),
        bin_op("sll",  0b001, 0, 171, [] (uint32_t a, uint32_t b) { return a << (b & 0x1F); }),
        bin_op("slt",  0b010, 0, 138, [] (uint32_t a, uint32_t b) { return static_cast<int32_t>(a) < static_cast<int32_t>(b); }),
        bin_op("sltu", 0b011, 0, 138, [] (uint32_t a, uint32_t b) { return a < b; }),
        bin_op("xor",  0b100, 0, 105, [] (uint32_t a, uint32_t b) { return a ^ b; }),
        bin_op("srl",  0b101, 0, 171, [] (uint32_t a, uint32_t b) { return a >> (b & 0x1F); }),
        bin_op("sra",  0b101, 1, 171, [] (uint32_t a, uint32_t b) { return static_cast<uint32_t>(static_cast<int32_t>(a) >> (b & 0x1F)); }),
        bin_op("or",   0b110, 0, 105, [] (uint32_t a, uint32_t b) { return a | b; }),
        bin_op("and",  0b111, 0, 105, [] (uint32_t a, uint32_t b) { return a & b; }),

        bin_imm_op("addi",  0b000, 105, [] (uint32_t a, uint32_t b) { return a + b; }),
        bin_imm_op("slti",  0b010, 138, [] (uint32_t a, uint32_t b) { return static_cast<int32_t>(a) < static_cast<int32_t>(b); }),
        bin_imm_op("sltui", 0b011, 138, [] (uint32_t a, uint32_t b) { return a < b; }),
        bin_imm_op("xori",  0b100, 105, [] (uint32_t a, uint32_t b) { return a ^ b; }),
        bin_imm_op("ori",   0b110, 105, [] (uint32_t a, uint32_t b) { return a | b; }),
        bin_imm_op("andi",  0b111, 105, [] (uint32_t a, uint32_t b) { return a & b; }),

        shift_imm_op("slli",  0b001, 0, 171, [] (uint32_t a, uint32_t b) { return a << (b & 0x1F); }),
        shift_imm_op("srli",  0b101, 0, 171, [] (uint32_t a, uint32_t b) { return a >> (b & 0x1F); }),
        shift_imm_op("srai",  0b101, 1, 171, [] (uint32_t a, uint32_t b) { return static_cast<uint32_t>(static_cast<int32_t>(a) >> (b & 0x1F)); }),

        branch_op("beq",  0b000, 106, [] (uint32_t a, uint32_t b) { return a == b; }),
        branch_op("bne",  0b001, 106, [] (uint32_t a, uint32_t b) { return a != b; }),
        branch_op("blt",  0b100, 106, [] (uint32_t a, uint32_t b) { return static_cast<int32_t>(a) < static_cast<int32_t>(b); }),
        branch_op("bge",  0b101, 106, [] (uint32_t a, uint32_t b) { return static_cast<int32_t>(a) >= static_cast<int32_t>(b); }),
        branch_op("bltu", 0b110, 106, [] (uint32_t a, uint32_t b) { return a < b; }),
        branch_op("bgeu", 0b111, 106, [] (uint32_t a, uint32_t b) { return a >= b; }),

        lui_op("lui", 105),
        auipc_op("auipc", 105),

        jal_op("jal", 139),
        jalr_op("jalr", 139),

        load_op("lb",  0b000, 140, [] (Memory &memory, uint32_t address) { return memory.read_int8(address); }),
        load_op("lh",  0b001, 141, [] (Memory &memory, uint32_t address) { return memory.read_int16(address); }),
        load_op("lw",  0b010, 143, [] (Memory &memory, uint32_t address) { return memory.read_uint32(address); }),
        load_op("lbu", 0b100, 140, [] (Memory &memory, uint32_t address) { return memory.read_uint8(address); }),
        load_op("lhu", 0b101, 141, [] (Memory &memory, uint32_t address) { return memory.read_uint16(address); }),

        store_op("sb", 0b000, 138, [] (Memory &memory, uint32_t address, uint32_t data) { memory.write_uint8(address, data); }),
        store_op("sh", 0b001, 139, [] (Memory &memory, uint32_t address, uint32_t data) { memory.write_uint16(address, data); }),
        store_op("sw", 0b010, 141, [] (Memory &memory, uint32_t address, uint32_t data) { memory.write_uint32(address, data); }),

        nop_op("fence.i", 0b00011, 0b001, OpCodeTable::any, OpCodeTable::any, 73),
        nop_op("fence",   0b00011, 0b000, OpCodeTable::any, OpCodeTable::any, 73),

        csr_op("csrrw", 0b001, 137, [] (uint32_t a, uint32_t b) { return a; }),
        csr_op("csrrs", 0b010, 137, [] (uint32_t a, uint32_t b) { return b | a; }),
        csr_op("csrrc", 0b011, 137, [] (uint32_t a, uint32_t b) { return b & ~a; }),

        csr_imm_op("csrrwi", 0b101, 137, [] (uint32_t a, uint32_t b) { return a; }),
        csr_imm_op("csrrsi", 0b110, 137, [] (uint32_t a, uint32_t b) { return b | a; }),
        csr_imm_op("csrrci", 0b111, 137, [] (uint32_t a, uint32_t b) { return b & ~a; }),

        trap_op("trap", 105),
        mret_op("mret", 105),
        wfi_op("wfi", 73),
    };

    csr.add(0x300, "mstatus", std::make_shared<MaskedRegister>(0x0088, 0x1800));
    csr.add(0x304, "mie", std::make_shared<MaskedRegister>(0x000E0000));
    csr.add(0x305, "mtvec", std::make_shared<ValueRegister>());
    csr.add(0x340, "mscratch", std::make_shared<ValueRegister>());
    csr.add(0x341, "mepc", std::make_shared<ValueRegister>());
    csr.add(0x342, "mcause", std::make_shared<ValueRegister>());
    csr.add(0x344, "mip", std::make_shared<MaskedRegister>(0x000E0000));

    csr.add(0xB82, "minstreth", std::make_shared<ValueRegister>());
    csr.add(0xB02, "minstret", std::make_shared<CarriedRegister>(csr["minstreth"]));

    csr.add(0xB80, "mcycleh", std::make_shared<ValueRegister>());
    csr.add(0xB00, "mcycle", std::make_shared<CarriedRegister>(csr["mcycleh"]));
}

void CPU::step()
{
    fetch();
    exec();
}

void CPU::fetch()
{
    uint32_t instruction = memory.read_uint32(pc);
    opcode = opcode_table.decode(instruction);

    if(!sleeping) {
        on_opcode_decode(pc, instruction, opcode->format(*this));
    }
}

void CPU::exec()
{
    opcode->exec(*this);

    if(!sleeping) {
        Register& minstret = csr["minstret"];
        if(minstret == 0xFFFFFFFF) {
        }
        minstret += 1;
    }
}

ConstRegister CSR::dummy{0};


/*
BEQ [Taken]: 140 substeps
BEQ [Not taken]: 106 substeps
BNE [Taken]: 140 substeps
BNE [Not taken]: 106 substeps
BLT [Taken]: 140 substeps
BLT [Not taken]: 106 substeps
BGE [Taken]: 140 substeps
BGE [Not taken]: 106 substeps
BLTU [Taken]: 140 substeps
BLTU [Not taken]: 106 substeps
BGEU [Taken]: 140 substeps
BGEU [Not taken]: 106 substeps
JAL: 139 substeps
JALR: 139 substeps
LB: 140 substeps
LH: 141 substeps
LW: 143 substeps
LBU: 140 substeps
LHU: 141 substeps
SB: 138 substeps
SH: 139 substeps
SW: 141 substeps
ADDI: 105 substeps
XORI: 105 substeps
ORI: 105 substeps
ANDI: 105 substeps
SLLI: 171 substeps
SRLI: 171 substeps
SRAI: 171 substeps
SLL: 171 substeps
SRL: 171 substeps
SRA: 171 substeps
SLTIU [True]: 138 substeps
SLTIU [False]: 138 substeps
SLTI [True]: 138 substeps
SLTI [False]: 138 substeps
ADD: 105 substeps
SUB: 105 substeps
XOR: 105 substeps
OR: 105 substeps
AND: 105 substeps
SLTU [True]: 138 substeps
SLTU [False]: 138 substeps
SLT [True]: 138 substeps
SLT [False]: 138 substeps
CSRRW: 137 substeps
CSRRS: 137 substeps
CSRRC: 137 substeps
CSRRWI: 137 substeps
CSRRSI: 137 substeps
CSRRCI: 137 substeps
IllOp: 74 substeps
FENCE: 73 substeps
FENCE.I: 73 substeps
TRAP: 105 substeps
MRET: 105 substeps
WFI: 73 substeps
*/
