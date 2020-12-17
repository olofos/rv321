#include <sstream>
#include <iomanip>

#include "csr.h"

Register& CSR::operator[] (uint16_t address)
{
    try {
        return *address_to_reg.at(address);
    } catch(...) {
        std::stringstream ss;
        ss << "Warning: Unknown CSR address 0x" << std::setfill('0') << std::setw(3) << std::uppercase << std::hex << address;
        on_info(ss.str());
        return dummy;
    }
}

Register& CSR::operator[] (const std::string &name)
{
    try {
        return *name_to_reg.at(name);
    } catch(...) {
        std::stringstream ss;
        ss << "Warning: Unknown CSR " << name;
        on_info(ss.str());
        return dummy;
    }
}

std::string CSR::get_name(uint16_t address) const
{
    try {
        return address_to_name.at(address);
    } catch(...) {
        std::stringstream ss;
        ss << "[" << std::setfill('0') << std::setw(3) << std::uppercase << std::hex << address << "]";
        return ss.str();
    }
}
