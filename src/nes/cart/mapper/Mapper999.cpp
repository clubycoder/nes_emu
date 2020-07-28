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
Custom mapper used for development and debugging

CPU:
  0x8000 -> 0xFFFF: Map 0x0000 -> 0x7FFF
  Writes to 0xFF01 outputs character to stdout

PPU:
  0x0000 -> 0x1FFF: No mapping is required
*******************************************************************************/

#include <cstdint>

#include <nes/cart/mapper/Mapper999.hpp>
#include <nes/cart/Cart.hpp>

namespace nes { namespace cart { namespace mapper {

void Mapper999::reset() {
    // Nothing to do on reset
}

const bool Mapper999::cpu_map_read_addr(const uint16_t addr, uint32_t &mapped_addr) {
    if (addr >= ADDR_PRG_ROM_BEGIN && addr <= ADDR_PRG_ROM_END) {
        mapped_addr = addr & 0x7FFF;
        return true;
    }

    return false;
}

const bool Mapper999::cpu_map_write_addr(const uint16_t addr, uint32_t &mapped_addr, const uint8_t data) {
    // If the write is going to the special character printing address,
    // write the data out as a single character to stdout and flush.
    if (addr == ADDR_PRG_PRINT_CHAR) {
        mapped_addr = addr;
        std::cout << (char)data << std::flush;
        return true;
    }

    return false;
}

const bool Mapper999::ppu_map_read_addr(const uint16_t addr, uint32_t &mapped_addr) {
    if (addr >= ADDR_CHR_ROM_BEGIN && addr <= ADDR_CHR_ROM_END) {
        mapped_addr = addr;
        return true;
    }

    return false;
}

const bool Mapper999::ppu_map_write_addr(const uint16_t addr, uint32_t &mapped_addr, const uint8_t data) {
    return false;
}

}}} // nes::cart::mapper