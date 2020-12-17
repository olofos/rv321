#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sstream>

#include "term.h"

Term::Term(std::function<void(const std::string&)> on_write_info_) : on_write_info(on_write_info_), read_time{0,0}, write_time{0,0}
{
    fd = posix_openpt(O_RDWR | O_NONBLOCK);
    grantpt(fd);
    unlockpt(fd);

    struct termios tmios;
    tcgetattr(fd, &tmios);
    tmios.c_lflag &= ~(ECHO);
    tcsetattr(fd, TCSANOW, &tmios);

    const char *s = ptsname(fd);
    std::stringstream ss;
    ss << "Terminal opened at " << s;
    on_write_info(ss.str());
}

void Term::write(uint8_t data)
{
    write_fifo.push_back(data);
}

uint8_t Term::read()
{
    if(!read_fifo.empty()) {
        char c = read_fifo.front();
        read_fifo.pop_front();

        return c;
    } else {
        return 0;
    }
}

void Term::update()
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    if((now.tv_sec - read_time.tv_sec) + 1.0e-9 * (now.tv_nsec - read_time.tv_nsec) > 10.0 / baud) {
        uint8_t buf[1];
        int ret = ::read(fd, buf, sizeof(buf));

        if(ret > 0 && !read_fifo.empty()) {
            std::stringstream ss;
            ss << "Read fifo not empty (" << read_fifo.size() << ")";
            on_write_info(ss.str());
        }


        for(int i = 0; i < ret; i++) {
            read_fifo.push_back(buf[i]);
        }
        read_time = now;
    }


    if((now.tv_sec - write_time.tv_sec) + 1.0e-9 * (now.tv_nsec - write_time.tv_nsec) > 10.0 / baud) {
        if(!write_fifo.empty()) {
            char data = write_fifo.front();
            if(data == '\n') {
                const char *buf = "\r\n";
                auto ret = ::write(fd, buf, 2);
                (void)ret;
            } else {
                auto ret = ::write(fd, &data, 1);
                (void)ret;
            }
            write_fifo.pop_front();
        }
        write_time = now;
    }
}
