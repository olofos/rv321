#ifndef LCD_H_
#define LCD_H_

#include <SFML/Graphics.hpp>
#include <vector>
#include <thread>
#include <mutex>
#include <functional>
#include <atomic>

typedef struct vrEmuLcd_s VrEmuLcd;

class LCDEmu
{
    VrEmuLcd *vrLcd = nullptr;

    LCDEmu(const LCDEmu&) = delete;
    LCDEmu& operator= (const LCDEmu&) = delete;

public:
    enum PixelState { BG = 0, ON = 1, OFF = 2, };
    enum Font { Japanese, European };

    LCDEmu() {}
    LCDEmu(int w, int h, enum Font font = LCDEmu::European);
    LCDEmu(LCDEmu&& that) { vrLcd = that.vrLcd; that.vrLcd = nullptr; }
    ~LCDEmu();

    int width() const;
    int height() const;

    void write_command(uint8_t d);
    void write_data(uint8_t d);

    uint8_t read_address() const;
    uint8_t read_data() const;

    void render(std::function<void(int,int,PixelState)>);
};

class LCD {
    LCDEmu lcd;

    const int pixel_size = 10;
    const int margin = 10;

    const int pixel_border_top = 1;
    const int pixel_border_bottom = 1;
    const int pixel_border_left = 1;
    const int pixel_border_right = 1;

    const int width;
    const int height;

    const sf::Color on{60, 65, 44};
    const sf::Color off{111, 185, 0};
    const sf::Color bg{168, 198, 78};

    sf::RenderWindow window;
    std::vector<sf::RectangleShape> pixels;

    std::thread thread;
    std::mutex mutex;

    void run();
    void handle_events();
    void render();

    void draw_pixel(int x, int y, LCDEmu::PixelState state);
public:
    LCD(int w, int h);
    ~LCD();

    void write_command(uint8_t d);
    void write_data(uint8_t d);

    uint8_t read_address();
    uint8_t read_data();

    bool is_closed();
};

#endif
