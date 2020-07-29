/*******************************************************************************
MIT License

Copyright (c) 2020 Chris Luby

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

/*******************************************************************************
Emulation of the NTSC 2C02 used as the Picture Processing Unit in the Nintendo
Entertainment System

Links:
- https://wiki.nesdev.com/w/index.php/PPU
*******************************************************************************/

#include <iostream>
#include <cstdint>

#include <nes/ppu/PPU2C02.hpp>

namespace nes { namespace ppu {

void PPU2C02::reset() {
    m_x = m_y = 0;
}

void PPU2C02::clock() {
    Component::clock();

    // std::cout << "PPU.clock: " << m_x << "," << m_y << std::endl;

    // If we're on the first, top-left pixel, open the screen
    if (m_x == 0 && m_y == 0) {
        open_screen();
    }

    uint8_t r = m_y;
    uint8_t g = m_x;
    uint8_t b = m_y * m_x + m_clock_count;
    set_pixel(m_x, m_y, r, g, b);

    // Each clock cycle will draw one pixel from top-left to bottom-right
    m_x++;
    // If we are at the end of the internal scanline (overscan by 85 pixels on the right)...
    if (m_x >= SCREEN_WIDTH_INTERNAL) {
        m_x = 0;
        m_y++;
        // If we are at the end of the internal screen (overscan by 22 scanlines on the bottom)...
        if (m_y >= SCREEN_HEIGHT_INTERNAL) {
            m_y = 0;
            close_screen();
        }
    }
}

const bool PPU2C02::cpu_read(const uint16_t addr, uint8_t &data, const bool read_only) {
    data = 0x00;

    return true;
}

const bool PPU2C02::cpu_write(const uint16_t addr, const uint8_t data) {
    return true;
}

}} // nes::ppu