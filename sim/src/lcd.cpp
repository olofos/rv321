#include "lcd.h"
#include "vrEmuLcd.h"

#include <sstream>

LCDEmu::LCDEmu(int w, int h, Font font)
{
    vrLcd = vrEmuLcdNew(w, h, (font == European) ? EmuLcdRomA02 : EmuLcdRomA00);
}

LCDEmu::~LCDEmu()
{
    if(vrLcd) vrEmuLcdDestroy(vrLcd);
    vrLcd = 0;
}

int LCDEmu::width() const
{
    return vrEmuLcdNumPixelsX(vrLcd);
}

int LCDEmu::height() const
{
    return vrEmuLcdNumPixelsY(vrLcd);
}

void LCDEmu::write_command(uint8_t d)
{
    vrEmuLcdSendCommand(vrLcd, d);
}

void LCDEmu::write_data(uint8_t d)
{
    vrEmuLcdWriteByte(vrLcd, d);
}

uint8_t LCDEmu::read_address() const
{
    return vrEmuLcdReadAddress(vrLcd);
}

uint8_t LCDEmu::read_data() const
{
    return vrEmuLcdReadByte(vrLcd);
}

void LCDEmu::render(std::function<void(int,int,PixelState)> render_pixel)
{
    vrEmuLcdUpdatePixels(vrLcd);

    const int width = this->width();
    const int height = this->height();

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            enum PixelState state = BG;
            int s = vrEmuLcdPixelState(vrLcd, x, y);
            if(s == +1) {
                state = ON;
            } else if(s == 0) {
                state = OFF;
            }
            render_pixel(x, y, state);
        }
    }
}

LCD::LCD(int w, int h) :
    lcd(w,h),
    width(lcd.width()),
    height(lcd.height()),
    window(sf::VideoMode(2*margin + pixel_size*width - pixel_border_left - pixel_border_right, 2*margin + pixel_size*height - pixel_border_top - pixel_border_bottom), "LCD", sf::Style::Close),
    pixels(width * height)
{
    std::stringstream ss;
    ss << "LCD (" << w << " x " << h << ")";
    window.setTitle(ss.str());
    window.setVisible(false);

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            pixels[width*y + x].setPosition(margin + pixel_size * x + pixel_border_left, margin + pixel_size * y + pixel_border_top);
            pixels[width*y + x].setSize(sf::Vector2f(pixel_size - pixel_border_left - pixel_border_right, pixel_size - pixel_border_top - pixel_border_bottom));
        }
    }

    thread = std::thread(&LCD::run, this);
}

LCD::~LCD()
{
    {
        const std::lock_guard<std::mutex> lock(mutex);
        if(window.isOpen()) {
            window.close();
        }
    }
    thread.join();
}

bool LCD::is_closed()
{
    const std::lock_guard<std::mutex> lock(mutex);
    return !window.isOpen();
}

void LCD::handle_events()
{
    const std::lock_guard<std::mutex> lock(mutex);
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed) {
            window.close();
        } else if((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) {
            window.close();
        }
    }
}

void LCD::render()
{
    const std::lock_guard<std::mutex> lock(mutex);
    window.clear(bg);
    lcd.render([&] (int x, int y, LCDEmu::PixelState state) { this->draw_pixel(x, y, state); });
    window.display();
}

void LCD::run()
{
    while(!is_closed()) {
        handle_events();
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void LCD::write_command(uint8_t d)
{
    const std::lock_guard<std::mutex> lock(mutex);
    lcd.write_command(d);
    window.setVisible(true);
}

void LCD::write_data(uint8_t d)
{
    const std::lock_guard<std::mutex> lock(mutex);
    lcd.write_data(d);
    window.setVisible(true);
}

uint8_t LCD::read_address()
{
    const std::lock_guard<std::mutex> lock(mutex);
    return lcd.read_address();
}

uint8_t LCD::read_data()
{
    const std::lock_guard<std::mutex> lock(mutex);
    return lcd.read_data();
}


void LCD::draw_pixel(int x, int y, LCDEmu::PixelState state) {
    if(state == LCDEmu::ON) {
        pixels[width*y + x].setFillColor(on);
    } else if(state == LCDEmu::OFF) {
        pixels[width*y + x].setFillColor(off);
    } else {
        pixels[width*y + x].setFillColor(bg);
    }
    window.draw(pixels[width*y + x]);
}
