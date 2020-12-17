#ifndef OPCODE_TABLE_H_
#define OPCODE_TABLE_H_

#include <iostream>
#include <iomanip>
#include <functional>

class OpCode;
class OpCodeTableEntry;

class OpCodeTable
{
    std::vector<std::shared_ptr<OpCodeTableEntry>> table;
public:
    OpCodeTable() {}
    OpCodeTable(std::initializer_list<std::shared_ptr<OpCodeTableEntry>> init_list) : table(init_list) {};

    std::shared_ptr<OpCode> decode(uint32_t instruction) const;

    static constexpr uint32_t get_compact_instruction(uint32_t opclass, uint32_t func3, uint32_t sa, uint32_t sys)
        {
            return ((sys & 01) << 9) | ((sa & 01) << 8) | ((func3 & 07) << 5) | (opclass & 037);
        }

    static constexpr uint32_t get_compact_instruction_mask(uint32_t opclass, uint32_t func3, uint32_t sa, uint32_t sys)
        {
            return get_compact_instruction(
                opclass == OpCodeTable::any ? 000 : 037,
                func3 == OpCodeTable::any ? 00 : 07,
                sa == OpCodeTable::any ? 0 : 1,
                sys == OpCodeTable::any ? 0 : 1);
        }

    static constexpr uint32_t get_compact_instruction(uint32_t inst)
        {
            uint32_t opclass = (inst >> 2) & 037;
            uint32_t func3 = (inst >> 12) & 07;
            uint32_t sa = ((inst >> 29) & 01) | ((inst >> 30) & 01);
            uint32_t sys = ((inst >> 25) & 01) | ((inst >> 28) & 01);

            return get_compact_instruction(opclass, func3, sa, sys);
        }

    static const uint32_t any = 0xFF;
};

class OpCodeTableEntry
{
    const uint32_t compact_pattern;
    const uint32_t compact_pattern_mask;
    std::function<std::shared_ptr<OpCode>(uint32_t,uint32_t)> opcode_factory;

public:
    OpCodeTableEntry(std::string name_, uint32_t opclass, uint32_t func3, uint32_t sa, uint32_t sys, std::function<std::shared_ptr<OpCode>(uint32_t,uint32_t)> opcode_factory_)
        : compact_pattern(OpCodeTable::get_compact_instruction(opclass, func3, sa, sys)),
          compact_pattern_mask(OpCodeTable::get_compact_instruction_mask(opclass, func3, sa, sys)),
          opcode_factory(opcode_factory_)
        {
        }

    bool match(uint32_t compact_instruction) { return (compact_instruction & compact_pattern_mask) == (compact_pattern & compact_pattern_mask); }
    std::shared_ptr<OpCode> decode(uint32_t instruction, uint32_t raw_instruction) { return opcode_factory(instruction, raw_instruction); }

    static const uint32_t any = 0xFF;
};

#endif
