#ifndef REGISTER_H_
#define REGISTER_H_

#include <cstdint>
#include <iostream>

class Register
{
    virtual uint32_t get() const = 0;
    virtual void set(uint32_t) = 0;
    static void on_change_dummy(uint32_t) {}

public:
    Register() : on_change(on_change_dummy) {}
    virtual ~Register() {}

    Register& operator =(const Register& r) {
        *this = static_cast<uint32_t>(r);
        return *this;
    }

    Register& operator =(uint32_t val) {
        uint32_t old_val = get();
        set(val);
        uint32_t new_val = get();
        if(new_val != old_val) {
            on_change(new_val);
        }
        return *this;
    }

    virtual Register& operator +=(uint32_t val) {
        *this = *this + val;
        return *this;
    }

    Register& operator |=(uint32_t val) {
        *this = *this | val;
        return *this;
    }

    Register& operator &=(uint32_t val) {
        *this = *this & val;
        return *this;
    }

    operator uint32_t() const { return get(); }

    std::function<void(uint32_t)> on_change;
};

class ValueRegister : public Register
{
    uint32_t value;
    virtual uint32_t get() const { return value; }
    virtual void set(uint32_t val) { value = val; }
public:
    ValueRegister() : value{} {}
    explicit ValueRegister(uint32_t val) : value{val} {}
    virtual ~ValueRegister() {}
};

class ConstRegister : public Register
{
    const uint32_t value;
    virtual uint32_t get() const { return value; }
    virtual void set(uint32_t) { };
public:
    ConstRegister(uint32_t val) : value{val} {}
    virtual ~ConstRegister() {}
};

class MaskedRegister : public Register
{
    const uint32_t mask;
    const uint32_t fixed;
    uint32_t value;

    virtual uint32_t get() const { return (value & mask) | fixed; }
    virtual void set(uint32_t val) { value = val; };
public:
    MaskedRegister(uint32_t mask_, uint32_t fixed_ = 0, uint32_t value_ = 0) : mask{mask_}, fixed{fixed_}, value{value_} {}
    virtual ~MaskedRegister() {}
};

class CarriedRegister : public Register
{
    uint32_t value;
    Register& hi;
    virtual uint32_t get() const { return value; }
    virtual void set(uint32_t val) { value = val; }
public:
    CarriedRegister(Register& hi_) : value{}, hi(hi_) {}

    virtual Register& operator +=(uint32_t val) {
        if(static_cast<uint64_t>(value) + val > 0xFFFFFFFF) {
            hi += 1;
        }
        *static_cast<Register*>(this) = value + val;
        return *this;
    }
};


#endif
