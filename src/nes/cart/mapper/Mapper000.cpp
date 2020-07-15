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

CPU:
  NROM-128:
	  0x8000 -> 0xBFFF: Map    0x0000 -> 0x3FFF
	  0xC000 -> 0xFFFF: Mirror 0x0000 -> 0x3FFF
  NROM-256:
    0x8000 -> 0xFFFF: Map    0x0000 -> 0x7FFF

PPU:
  0x0000 -> 0x1FFF: No mapping is required
*******************************************************************************/

#include <cstdint>

#include <nes/cart/mapper/Mapper000.hpp>

namespace nes { namespace cart { namespace mapper {

void Mapper000::reset() {
    // Nothing to do on reset
}

const bool Mapper000::cpu_map_read_addr(const uint16_t addr, uint32_t &mapped_addr) {
    if (addr >= ADDR_PRG_ROM_BEGIN && addr <= ADDR_PRG_ROM_END) {
        mapped_addr = addr & (m_variation == NROM_256 ? 0x7FFF : 0x3FFF);
        return true;
    }

    return false;
}

const bool Mapper000::cpu_map_write_addr(const uint16_t addr, uint32_t &mapped_addr, const uint8_t data) {
    // Write mapping is the same as read
    return cpu_map_read_addr(addr, mapped_addr);
}

const bool Mapper000::ppu_map_read_addr(const uint16_t addr, uint32_t &mapped_addr) {
    if (addr >= ADDR_CHR_ROM_BEGIN && addr <= ADDR_CHR_ROM_END) {
        mapped_addr = addr;
        return true;
    }

    return false;
}

const bool Mapper000::ppu_map_write_addr(const uint16_t addr, uint32_t &mapped_addr, const uint8_t data) {
    // Only map if CHR is RAM
    if (m_num_chr_banks == 0) {
        // Write mapping is the same as read
        return ppu_map_read_addr(addr, mapped_addr);
    }

    return false;
}

}}} // nes::cart::mapper