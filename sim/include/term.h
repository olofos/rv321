#ifndef TERM_H_
#define TERM_H_

#include <cstdint>
#include <functional>
#include <deque>
#include <string>
#include <ctime>

class Term
{
    std::function<void(const std::string&)> on_write_info;
    std::deque<uint8_t> read_fifo;
    std::deque<uint8_t> write_fifo;
    int fd;

    timespec read_time;
    timespec write_time;

    static const int baud = 2400;
public:
    Term(std::function<void(const std::string&)> on_write_info_);
    uint8_t read();
    void write(uint8_t data);
    void update();
    bool empty() { return read_fifo.empty(); }
};


#endif
