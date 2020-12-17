#ifndef MEMORY_H_
#define MEMORY_H_

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <functional>

class MemoryRegion
{
protected:
    static void on_info_dummy(const std::string&) {}
public:
    uint32_t start;
    uint32_t len;

    MemoryRegion(uint32_t start_, uint32_t len_, std::function<void(const std::string&)> on_info_ = on_info_dummy) : start(start_), len(len_), on_info(on_info_) {}

    virtual uint8_t read(uint32_t address) = 0;
    virtual void write(uint32_t address, uint8_t data) = 0;

    bool includes(uint32_t address) { return (start <= address) && (address < start + len); }

    std::function<void(const std::string&)> on_info;
};

class RAM : public MemoryRegion
{
    std::vector<uint8_t> buf;
public:
    RAM(uint32_t address, uint32_t size) : MemoryRegion(address, size), buf(size) {}
    virtual uint8_t read(uint32_t address) { return buf[address-start]; }
    virtual void write(uint32_t address, uint8_t data) { buf[address-start] = data; }
};

class ROM : public MemoryRegion
{
    std::vector<uint8_t> buf;
public:
    ROM(uint32_t address, const std::string filename, std::function<void(const std::string&)> on_info_ = on_info_dummy);
    virtual uint8_t read(uint32_t address) { return buf[address-start]; }
    virtual void write(uint32_t address, uint8_t data);
};

class MemoryRegister : public MemoryRegion
{
    std::function<uint8_t()> on_read;
    std::function<void(uint8_t)> on_write;
public:
    MemoryRegister(uint32_t address, std::function<uint8_t()> on_read_, std::function<void(uint8_t)> on_write_, std::function<void(const std::string&)> on_info_ = on_info_dummy)
        : MemoryRegion(address, 1, on_info_), on_read(on_read_), on_write(on_write_) {}
    virtual uint8_t read(uint32_t address) { return on_read(); }
    virtual void write(uint32_t address, uint8_t data) { on_write(data); }
};

class MemoryRegisterRO : public MemoryRegister
{
    void on_write_ro(uint8_t);
public:
    MemoryRegisterRO(uint32_t address, std::function<uint8_t()> on_read_, std::function<void(const std::string&)> on_info_ = on_info_dummy)
        : MemoryRegister(address, on_read_, [&] (uint8_t data) { on_write_ro(data); }, on_info_) {}
};

class MemoryRegisterWO : public MemoryRegister
{
    uint8_t on_read_wo();
public:
    MemoryRegisterWO(uint32_t address, std::function<void(uint8_t)> on_write_, std::function<void(const std::string&)> on_info_ = on_info_dummy)
        : MemoryRegister(address, [&] () { return on_read_wo(); }, on_write_, on_info_) {}
};

class Memory
{
    std::vector<std::shared_ptr<MemoryRegion>> memory_map;

public:
    Memory(std::function<void(const std::string&)> on_info_) : on_info(on_info_) {}

    uint32_t read_uint8(uint32_t address) const;
    uint32_t read_uint16(uint32_t address) const;
    uint32_t read_uint32(uint32_t address) const;

    uint32_t read_int8(uint32_t address) const;
    uint32_t read_int16(uint32_t address) const;

    void write_uint8(uint32_t address, uint8_t value);
    void write_uint16(uint32_t address, uint16_t value);
    void write_uint32(uint32_t address, uint32_t value);

    void add(std::shared_ptr<MemoryRegion> reg) { memory_map.push_back(reg); }

    void load_file(std::string filename, uint32_t address);

    std::function<void(const std::string&)> on_info;
};

#endif
