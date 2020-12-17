#ifndef CSR_H_
#define CSR_H_

#include <cstdint>
#include <memory>
#include <map>
#include <vector>
#include <functional>

#include "register.h"

class CSR
{
    std::map<uint16_t,std::shared_ptr<Register>> address_to_reg;
    std::map<std::string,std::shared_ptr<Register>> name_to_reg;
    std::map<uint16_t,std::string> address_to_name;

    static ConstRegister dummy;
public:
    CSR(std::function<void(const std::string&)> on_info_) : on_info(on_info_) {}

    void add(uint16_t address, std::string name, std::shared_ptr<Register> reg) {
        address_to_reg[address] = reg;
        name_to_reg[name] = reg;
        address_to_name[address] = name;
    }

    Register& operator[] (uint16_t address);
    Register& operator[] (const std::string &name);

    std::string get_name(uint16_t address) const;

    std::map<std::string,std::shared_ptr<Register>>::iterator begin() { return name_to_reg.begin(); }
    std::map<std::string,std::shared_ptr<Register>>::iterator end() { return name_to_reg.end(); }

    std::function<void(const std::string&)> on_info;
};

#endif
