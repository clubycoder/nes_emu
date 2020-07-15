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
Emulation of the Mapper 000 / NROM chip on some of the the Nintendo Entertainment
System Cartriges

Links:
- https://wiki.nesdev.com/w/index.php/NROM
*******************************************************************************/

#pragma once

#include <cstdint>

#include <nes/cart/mapper/Mapper.hpp>

namespace nes { namespace cart { namespace mapper {

class Mapper000 : public Mapper {
public:
    Mapper000(uint8_t num_prg_banks, uint8_t num_chr_banks)
        : Mapper(num_prg_banks, num_chr_banks) {
        m_variation = num_prg_banks > 1 ? NROM_256 : NROM_128;
    }

    void reset() override;

    // Map addresses in CPU read/write address space
    const bool cpu_map_read_addr(const uint16_t addr, uint32_t &mapped_addr) override;
    const bool cpu_map_write_addr(const uint16_t addr, uint32_t &mapped_addr, const uint8_t data) override;

    // Map addresses in PPU read/write address space
    const bool ppu_map_read_addr(const uint16_t addr, uint32_t &mapped_addr) override;
    const bool ppu_map_write_addr(const uint16_t addr, uint32_t &mapped_addr, const uint8_t data) override;

private:
    const uint16_t ADDR_PRG_ROM_BEGIN = 0x8000; const uint16_t ADDR_PRG_ROM_END = 0xFFFF;
    const uint16_t ADDR_CHR_ROM_BEGIN = 0x0000; const uint16_t ADDR_CHR_ROM_END = 0x1FFF;

    enum Variation {
        NROM_128,
        NROM_256
    } m_variation;

};

}}} // nes::cart::mapper