#ifndef OPCODE_H_
#define OPCODE_H_

#include <cstdint>
#include <string>
#include <functional>

class CPU;
class Memory;

class OpCode
{
protected:
    const uint32_t instruction;
    const uint32_t raw_instruction;
    const uint8_t rs1;
    const uint8_t rs2;
    const uint8_t rd;
    const uint8_t len;
    const uint32_t cycles;
public:
    const std::string name;

    OpCode(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_)
        : instruction(instruction_),
          raw_instruction(raw_instruction_),
          rs1((instruction_ >> 15) & 0x1F),
          rs2((instruction_ >> 20) & 0x1F),
          rd((instruction_ >> 7) & 0x1F),
          len(((raw_instruction_ & 03) == 03) ? 4 : 2),
          cycles(cycles_),
          name(name_)
        {}

    virtual ~OpCode() {}

    virtual void exec(CPU &cpu) const {}
    virtual std::string format(const CPU&) const { return name; }

    uint32_t immI() const;
    uint32_t immS() const;
    uint32_t immB() const;
    uint32_t immU() const;
    uint32_t immJ() const;
    uint32_t immCSR() const;

};

class BinOp : public OpCode
{
    std::function<uint32_t(uint32_t,uint32_t)> func;
public:
    BinOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_, std::function<uint32_t(uint32_t,uint32_t)> func_) : OpCode(name_, instruction_, raw_instruction_, cycles_), func(func_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name + " x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", x" + std::to_string(rs2);
    }
};

class BinImmOp : public OpCode
{
    std::function<uint32_t(uint32_t,uint32_t)> func;
public:

    BinImmOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_, std::function<uint32_t(uint32_t,uint32_t)> func_) : OpCode(name_, instruction_, raw_instruction_, cycles_), func(func_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name + " x" + std::to_string(rd) + ", x" + std::to_string(rs1) + ", " + std::to_string(static_cast<int32_t>(immI()));
    }
};

class BranchOp : public OpCode
{
    std::function<bool(uint32_t,uint32_t)> comp;
public:
    BranchOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_, std::function<bool(uint32_t,uint32_t)> comp_) : OpCode(name_, instruction_, raw_instruction_, cycles_), comp(comp_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name + " x" + std::to_string(rs1) + ", x" + std::to_string(rs2) + ", " + std::to_string(static_cast<int32_t>(immB()));
    }
};

class LUIOp : public OpCode
{
public:
    LUIOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_) : OpCode(name_, instruction_, raw_instruction_, cycles_) {}

    virtual void exec(CPU &cpu) const;

    std::string format(const CPU&) const;
};

class AUIPCOp : public OpCode
{
public:
    AUIPCOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_) : OpCode(name_, instruction_, raw_instruction_, cycles_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name + " x" + std::to_string(rd) + ", " + std::to_string(static_cast<int32_t>(immU()));
    }
};

class JALOp : public OpCode
{
public:
    JALOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_) : OpCode(name_, instruction_, raw_instruction_, cycles_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name + " x" + std::to_string(rd) + ", " + std::to_string(static_cast<int32_t>(immJ()));
    }
};

class JALROp : public OpCode
{
public:
    JALROp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_) : OpCode(name_, instruction_, raw_instruction_, cycles_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name + " x" + std::to_string(rd) + ", " + std::to_string(static_cast<int32_t>(immI()))  + "(x" + std::to_string(rs1) + ")";
    }
};

class LoadOp : public OpCode
{
    std::function<uint32_t(Memory&,uint32_t)> read;
public:
    LoadOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_, std::function<uint32_t(Memory&,uint32_t)> read_) : OpCode(name_, instruction_, raw_instruction_, cycles_), read(read_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name + " x" + std::to_string(rd) + ", " + std::to_string(static_cast<int32_t>(immI()))  + "(x" + std::to_string(rs1) + ")";
    }
};

class StoreOp : public OpCode
{
    std::function<void(Memory&,uint32_t,uint32_t)> write;
public:
    StoreOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_, std::function<void(Memory&,uint32_t,uint32_t)> write_) : OpCode(name_, instruction_, raw_instruction_, cycles_), write(write_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name + " x" + std::to_string(rs2) + ", " + std::to_string(static_cast<int32_t>(immS()))  + "(x" + std::to_string(rs1) + ")";
    }
};

class NopOp : public OpCode
{
public:
    NopOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_) : OpCode(name_, instruction_, raw_instruction_, cycles_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name;
    }
};

class CSROp : public OpCode
{
    std::function<uint32_t(uint32_t,uint32_t)> op;
public:
    CSROp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_, std::function<uint32_t(uint32_t,uint32_t)> op_) : OpCode(name_, instruction_, raw_instruction_, cycles_), op(op_) {}

    virtual void exec(CPU &cpu) const;

    std::string format(const CPU&) const;
};

class CSRImmOp : public OpCode
{
    std::function<uint32_t(uint32_t,uint32_t)> op;
public:
    CSRImmOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_, std::function<uint32_t(uint32_t,uint32_t)> op_) : OpCode(name_, instruction_, raw_instruction_, cycles_), op(op_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name + " x" + std::to_string(rd) + ", " + std::to_string(static_cast<int32_t>(immI()) & 0xFFF) + ", " + std::to_string(rs1);
    }
};

class TrapOp : public OpCode
{
public:
    TrapOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_) : OpCode(name_, instruction_, raw_instruction_, cycles_) {}

    virtual void exec(CPU &cpu) const;

    std::string format(const CPU&) const;
};


class MRETOp : public OpCode
{
public:
    MRETOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_) : OpCode(name_, instruction_, raw_instruction_, cycles_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name;
    }
};

class WFIOp : public OpCode
{
public:
    WFIOp(std::string name_, uint32_t instruction_, uint32_t raw_instruction_, uint32_t cycles_) : OpCode(name_, instruction_, raw_instruction_, cycles_) {}

    virtual void exec(CPU &cpu) const;

    virtual std::string format(const CPU&) const {
        return name;
    }
};

#endif
