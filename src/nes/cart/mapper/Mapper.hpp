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

namespace nes { namespace cart { namespace mapper {

class Mapper {
public:
    Mapper(uint8_t num_prg_banks, uint8_t num_chr_banks)
        : m_num_prg_banks(num_prg_banks)
        , m_num_chr_banks(num_chr_banks) {
    }

    virtual void reset() = 0;

    // Map addresses in CPU read/write address space
    virtual const bool cpu_map_read_addr(const uint16_t addr, uint32_t &mapped_addr) = 0;
    virtual const bool cpu_map_write_addr(const uint16_t addr, uint32_t &mapped_addr, const uint8_t data) = 0;

    // Map addresses in PPU read/write address space
    virtual const bool ppu_map_read_addr(const uint16_t addr, uint32_t &mapped_addr) = 0;
    virtual const bool ppu_map_write_addr(const uint16_t addr, uint32_t &mapped_addr, const uint8_t data) = 0;

protected:
    uint8_t m_num_prg_banks;
    uint8_t m_num_chr_banks;
};

}}} // nes::cart::mapper
