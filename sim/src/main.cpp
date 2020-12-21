#include <iostream>
#include <fstream>
#include <iterator>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstdint>

#include <vector>
#include <deque>
#include <memory>
#include <string>
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <iomanip>

#include <bitset>

#include <curses.h>

#include <unistd.h>


#include "opcode.h"
#include "opcode-table.h"
#include "cpu.h"
#include "term.h"

#include "riscv_c2g_lut.h"

#include "lcd.h"

const uint32_t OpCodeTable::any;

class Window;

class Curses
{
    std::vector<Window*> windows;
    struct timespec prev_refresh;
public:
    Curses() : prev_refresh{0,0} {
        initscr();
        cbreak();
        noecho();
        curs_set(0);
        start_color();
        use_default_colors();
        ::refresh();
    }

    ~Curses() {
        endwin();
    }

    void add(Window& w) {
        windows.push_back(&w);
    }

    void refresh();
    void refresh_now();
};

class RegisterField;

class Window
{
protected:
    WINDOW* win;
    WINDOW* bwin;
public:
    Window(int h, int w, int y, int x) {
        bwin = newwin(h, w, y, x);
        win = newwin(h-2, w-2-2, y+1, x+1+1);

        init_pair(1, COLOR_WHITE, -1);
        init_pair(2, COLOR_YELLOW, -1);

        wattron(win, COLOR_PAIR(1));
        wattron(bwin, COLOR_PAIR(2));
        box(bwin, 0, 0);
        wrefresh(bwin);
    }

    Window(int h, int w, int y, int x, const char* name) : Window(h,w,y,x) {
        wattron(bwin, A_BOLD | COLOR_PAIR(2));
        mvwprintw(bwin, 0, 2, name);
        wrefresh(bwin);
    }

    void refresh() { wnoutrefresh(win); }

    virtual void reset() {}

    friend RegisterField;
};

void Curses::refresh_now()
{
    for(auto w : windows) {
        w->refresh();
    }
    ::refresh();

    clock_gettime(CLOCK_REALTIME, &prev_refresh);
}

void Curses::refresh()
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    if(((double)now.tv_sec + 1.0e-9*now.tv_nsec) - ((double)prev_refresh.tv_sec + 1.0e-9*prev_refresh.tv_nsec) > 1/10.) {
        refresh_now();
    }
}

std::stringstream log;

class RegisterField
{
    Window& win;
    int y;
    int x;
    uint32_t value;
    bool updated;

    void print() {
        if(updated) {
            wattron(win.win, A_REVERSE);
        }
        mvwprintw(win.win, y, x, "%08X", value);
        wattroff(win.win, A_REVERSE);
    }
public:
    RegisterField(Window& win_, int y_, int x_, Register &reg, std::string name) : win(win_), y(y_), x(x_), value(reg), updated(false) {
        mvwprintw(win.win, y, x - 2 - name.length(), "%s: ", name.c_str());

        reg.on_change = [this] (uint32_t val) { update(val); };
        print();
    }

    void update(uint32_t val) {
        value = val;
        updated = true;
        print();
    }

    void reset() {
        if(updated) {
            updated = false;
            print();
        }
    }

};

class RegisterWindow : public Window
{
    unsigned last_change_reg;
    uint32_t last_change_val;

    std::vector<std::shared_ptr<RegisterField>> reg_fields;
public:
    RegisterWindow(int h, int w, int y, int x) : Window(h,w,y,x,"Registers"), last_change_reg(0), last_change_val(0) {
    }

    void add_register(std::string name, Register& reg, int y, int x) {
        reg_fields.push_back(std::make_shared<RegisterField>(*this, y, x, reg, name));
    }

    virtual void reset() {
        for(auto r : reg_fields) {
            r->reset();
        }
    }
};

class InstructionWindow : public Window
{
public:
    InstructionWindow(int h, int w, int y, int x) : Window(h,w,y,x,"Instruction Log") {
        scrollok(win, TRUE);
        idlok(win, TRUE);
        wmove(win, 0, 0);
    }

    void update(uint32_t pc, uint32_t instr, const std::string& op) {
        std::stringstream ss;

        ss << std::setfill(' ') << std::setw(8) << std::uppercase << std::hex << pc << ":  ";
        if((instr & 03) == 03) {
            ss << std::setfill('0') << std::setw(8) << std::uppercase << std::hex << instr;
        } else {
            ss << std::setfill('0') << std::setw(4) << std::uppercase << std::hex << (instr & 0xFFFF) << "    " ;
        }
        ss << "  " << op;

        wprintw(win, "\n%s", ss.str().c_str());
    }

    std::function<void(uint32_t,uint32_t,const std::string&)> get_update_function() { return [&] (uint32_t pc, uint32_t instr, const std::string& op) { update(pc,instr,op); }; }
};


class InfoWindow : public Window
{
    std::string prev_message;
    int repeat_count;
public:
    InfoWindow(int h, int w, int y, int x) : Window(h,w,y,x,"Info"), repeat_count{} {
        scrollok(win, TRUE);
        idlok(win, TRUE);
        wmove(win, 0, 0);
    }

    void update(const std::string& s) {
        if(s == prev_message) {
            wprintw(win, "\r%s (x%d)", s.c_str(), repeat_count++);
        } else {
            wprintw(win, "\n%s", s.c_str());
            repeat_count = 1;
            prev_message = s;
        }
    }

    std::function<void(const std::string&)> get_update_function() { return [&] (const std::string& s) { update(s); }; }
};


std::shared_ptr<OpCode> OpCodeTable::decode(uint32_t raw_instruction) const
{
    uint32_t instruction;

    if((raw_instruction & 03) == 03) {
        instruction = raw_instruction;
    } else {
        raw_instruction &= 0x0000FFFF;

        if((raw_instruction & 03) == 0) {
            instruction = riscv_c2g_c0[raw_instruction >> 2];
        } else if((raw_instruction & 03) == 1) {
            instruction = riscv_c2g_c1[raw_instruction >> 2];
        } else {
            instruction = riscv_c2g_c2[raw_instruction >> 2];
        }
    }

    uint32_t compact_instruction = get_compact_instruction(instruction);

    for(auto op: table) {
        if(op->match(compact_instruction)) {
            return op->decode(instruction, raw_instruction);
        }
    }

    std::stringstream ss;
    ss << "Unknown instruction: " << std::setw(8) << std::setfill('0') << std::hex << instruction << " raw: " << std::setw(((raw_instruction & 3) == 3) ? 8 : 4) << std::setfill('0') << std::hex << raw_instruction;
    throw std::runtime_error(ss.str());
}

class InputFifo
{
    std::deque<char> fifo;
public:
    char pop() { if(!fifo.empty()) { char c = fifo.front(); fifo.pop_front(); return c; } else { return 0; } }
    void push(char c) { fifo.push_back(c); }
    bool empty() { return fifo.empty(); }
};

int main(int argc, char *argv[])
{
    Curses curses;

    InstructionWindow asmwin(LINES-2, COLS/2, 0, 0);
    RegisterWindow regwin(16, COLS/2, 0, COLS/2);
    InfoWindow infowin(LINES-16-2, COLS/2, 16, COLS/2);

    Term term(infowin.get_update_function());
    LCD lcd(20,4);

    curses.add(asmwin);
    curses.add(regwin);
    curses.add(infowin);

    CPU cpu(asmwin.get_update_function(), infowin.get_update_function());
    uint32_t start_address = 0;

    if(argc > 1) {
        uint32_t load_address = 0;
        if(argc > 3) {
            load_address = std::stol(argv[2], 0, 16);
            start_address = std::stol(argv[3], 0, 16);
        }
        cpu.memory.add(std::make_shared<RAM>(0, 0x20000));
        cpu.memory.load_file(argv[1], load_address);
    } else {
        cpu.memory.add(std::make_shared<ROM>(0, "../tests/term2.bin", infowin.get_update_function()));
    }
    cpu.memory.add(std::make_shared<MemoryRegisterWO>(0xC0000000, [&] (uint8_t c) { term.write(c); }, infowin.get_update_function()));
    cpu.memory.add(std::make_shared<MemoryRegisterRO>(0xC0000001, [&] () { return term.read(); }, infowin.get_update_function()));

    cpu.memory.add(std::make_shared<MemoryRegister>(0x40000000, [&] () { return lcd.read_address(); }, [&] (uint8_t c) { lcd.write_command(c); }));
    cpu.memory.add(std::make_shared<MemoryRegister>(0x40000001, [&] () { return lcd.read_data(); }, [&] (uint8_t c) { lcd.write_data(c); }));

    cpu.memory.add(std::make_shared<RAM>(0x80000000, 128 * 1024));

    for(unsigned i = 0; i < cpu.reg.nregs; i++) {
        std::stringstream ss;
        ss << "x" << i;
        regwin.add_register(ss.str(), cpu.reg[i], i/4, 16 * (i % 4) + 6);
    }

    regwin.add_register("pc", cpu.pc, 5, 23 - 8);

    {
        int i = 0;
        for(auto r : cpu.csr) {
            regwin.add_register(r.first, *(r.second), i/2 + 7, 32 * (i % 2) + 23 - 9);
            i++;
        }
    }

    cpu.pc = start_address;

    curses.refresh_now();

    try {
        bool breakpoint = true;
        uint64_t cycle_accum = 0;
        for(;;) {
            if(lcd.is_closed()) {
                break;
            }

            term.update();

            uint32_t prev_pc;

            if(!term.empty()) { cpu.csr["mip"] |= 0x00020000; }
            cpu.fetch();

            curses.refresh();

            if(cpu.opcode->name == "trap") {
                // breakpoint = true;
            }
            if(cpu.pc == (uint32_t)0x80) {
                // breakpoint = true;
            }
            if(breakpoint) {
                curses.refresh_now();
                nodelay(stdscr, 0);
                char c = getch();
                if(c == 'g') {
                    breakpoint = false;
                } else if(c == 'q') {
                    break;
                }
            } else {
                nodelay(stdscr, 1);
                int c = getch();
                if(c != ERR) {
                    if(c == 27) {
                        breakpoint = true;
                    }
                }
            }
            regwin.reset();
            cpu.exec();

            // if((cpu.opcode->name == "wfi") && !cpu.sleeping) breakpoint = true;

            if((cpu.pc == prev_pc) && !cpu.sleeping) {
                breakpoint = true;
            }

            prev_pc = cpu.pc;

            uint64_t cycle = (static_cast<uint64_t>(cpu.csr["mcycleh"]) << 32) | cpu.csr["mcycle"];
            while(cycle - cycle_accum > 1000) {
                usleep(1000 * 1000000.0 / cpu.freq);
                cycle_accum += 1000;
            }

            if(cpu.sleeping) {
                usleep(1000);
            }
        }

    } catch(std::runtime_error err) {
        infowin.update(err.what());
    }
    curses.refresh_now();
    nodelay(stdscr, 0);
    getch();
}
