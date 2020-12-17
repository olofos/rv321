#include <iostream>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <algorithm>

#include "memory.h"

uint32_t Memory::read_uint8(uint32_t address) const
{
    for(auto reg: memory_map) {
        if(reg->includes(address)) {
            uint8_t res = reg->read(address);
            return res;
        }
    }

    std::stringstream ss;
    ss << "Warning: Read from unmapped address 0x" << std::setfill('0') << std::setw(8) << std::hex << address;
    on_info(ss.str());
    return 0xFF;
}

void Memory::write_uint8(uint32_t address, uint8_t data)
{
    for(auto reg: memory_map) {
        if(reg->includes(address)) {
            reg->write(address, data);
            return;
        }
    }

    std::stringstream ss;
    ss << "Warning: Write to unmapped address 0x" << std::setfill('0') << std::setw(8) << std::hex << address;
    on_info(ss.str());
}

uint32_t Memory::read_uint16(uint32_t address) const
{
    uint32_t n0 = read_uint8(address);
    uint32_t n1 = read_uint8(address+1);
    return (n1 << 8) | n0;
}

uint32_t Memory::read_uint32(uint32_t address) const
{
    uint32_t n0 = read_uint8(address);
    uint32_t n1 = read_uint8(address+1);
    uint32_t n2 = read_uint8(address+2);
    uint32_t n3 = read_uint8(address+3);
    return (n3 << 24) | (n2 << 16) | (n1 << 8) | n0;
}

uint32_t Memory::read_int8(uint32_t address) const
{
    uint32_t res = read_uint8(address);
    uint32_t sign = (res & 0x80) ? 0xFFFFFF00 : 0x00000000;
    return sign | res;
}

uint32_t Memory::read_int16(uint32_t address) const
{
    uint32_t res = read_uint16(address);
    uint32_t sign = (res & 0x8000) ? 0xFFFF0000 : 0x00000000;
    return sign | res;
}

void Memory::write_uint16(uint32_t address, uint16_t data)
{
    write_uint8(address, data & 0x00FF);
    write_uint8(address+1, (data >> 8) & 0x00FF);
}

void Memory::write_uint32(uint32_t address, uint32_t data)
{
    write_uint8(address, data & 0x000000FF);
    write_uint8(address+1, (data >> 8) & 0x000000FF);
    write_uint8(address+2, (data >> 16) & 0x000000FF);
    write_uint8(address+3, (data >> 24) & 0x000000FF);
}

void Memory::load_file(std::string filename, uint32_t address)
{
    std::ifstream f(filename, std::ios::binary);
    f.unsetf(std::ios::skipws);
    auto in = std::istream_iterator<uint8_t>(f);
    auto end = std::istream_iterator<uint8_t>();

    uint32_t len = 0;

    while(in != end) {
        write_uint8(address + len, *in++);
        len++;
    }

    std::stringstream ss;
    ss << "Loaded " << len << " bytes from " << filename << " at " << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << address;
    on_info(ss.str());
}

ROM::ROM(uint32_t address, std::string filename, std::function<void(const std::string&)> on_info_) : MemoryRegion(address, 0, on_info_), buf(0)
{
    std::ifstream f(filename, std::ios::binary);
    f.unsetf(std::ios::skipws);
    std::copy(std::istream_iterator<uint8_t>(f),
              std::istream_iterator<uint8_t>(),
              std::back_inserter(buf));

    for(int i = 0; i < 128 * 1024; i++) {
        buf.push_back(0);
    }

    len = buf.size();

    std::stringstream ss;
    ss << "Loaded " << len << " bytes from " << filename << " at " << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << start;
    on_info(ss.str());
}

void ROM::write(uint32_t address, uint8_t data)
{
    std::stringstream ss;
    ss << "Warning: Write to ROM address 0x" << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << address;
    on_info(ss.str());

    buf[address-start] = data;
}

void MemoryRegisterRO::on_write_ro(uint8_t data)
{
    std::stringstream ss;
    ss << "Warning: Write to RO memory address 0x" << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << start;
    on_info(ss.str());
}

uint8_t MemoryRegisterWO::on_read_wo()
{
    std::stringstream ss;
    ss << "Warning: Read from WO memory address 0x" << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << start;
    on_info(ss.str());
    return 0xFF;
}
