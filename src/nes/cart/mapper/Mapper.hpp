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
Emulation of the Mapper chip on the Nintendo Entertainment System Cartrige that
intercepts / adstracts the system, cartrige, and PPU memory access
*******************************************************************************/

#pragma once

#include <cstdint>

#include <nes/Component.hpp>

namespace nes { namespace cart { namespace mapper {

class Mapper : public Component {
public:
    Mapper(uint8_t num_prg_banks, uint8_t num_chr_banks)
        : m_num_prg_banks(num_prg_banks)
        , m_num_chr_banks(num_chr_banks) {
    }
    ~Mapper();

    virtual void clock();

    // Manage read/write in CPU address space
    virtual bool cpu_read(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) = 0;
    virtual bool cpu_write(uint16_t addr, uint32_t &mapped_addr, uint8_t data) = 0;

    // Manage read/write in the PPU address space
    virtual bool ppu_read(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) = 0;
    virtual bool ppu_write(uint16_t addr, uint32_t &mapped_addr, uint8_t data) = 0;

private:
    uint8_t m_num_prg_banks;
    uint8_t m_num_chr_banks;
};

}}} // nes::cart::mapper
