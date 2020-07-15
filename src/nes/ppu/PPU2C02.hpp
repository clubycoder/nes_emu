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

#pragma once

#include <cstdint>

#include <nes/Component.hpp>

namespace nes { namespace ppu {

class PPU2C02 : public Component {
public:
    static const int SCREEN_WIDTH = 256;
    static const int SCREEN_HEIGHT = 240;

    PPU2C02() {
    }

    virtual void reset();

    virtual void clock();

protected:
    virtual void open_screen();
    virtual void close_screen();
    virtual void set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) = 0;

private:

};

}} // nes::ppu