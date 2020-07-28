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
Emulation of the Nintendo Entertainment System Cartridge

Links:
- https://wiki.nesdev.com/w/index.php/INES
*******************************************************************************/

#pragma once

#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <memory>

#include <nes/Component.hpp>
#include <nes/cart/Header.hpp>
#include <nes/cart/mapper/Mapper.hpp>
#include <nes/cart/mapper/Mapper999.hpp>

namespace nes { namespace cart {

class Cart : public Component, public std::enable_shared_from_this<Cart> {
public:
    static const uint32_t PRG_BANK_SIZE = 16 * 1024;
    static const uint32_t CHR_BANK_SIZE = 8 * 1024;

    Cart(const std::string &filename);
    Cart(const std::vector<uint8_t> &rom_memory, const uint16_t start_address);
    ~Cart();

    // Setup the mapper once the ID is known
    void setup_mapper();

    // Reset cartridge to a known state (mainly the mapper)
    void reset() override;

    // Handle read/write from CPU bus
    const bool cpu_read(const uint16_t addr, uint8_t &data, const bool read_only = false) override;
    const bool cpu_write(const uint16_t addr, const uint8_t data) override;

    // Handle read/write from PPU bus
    const bool ppu_read(const uint16_t addr, uint8_t &data, const bool read_only = false);
    const bool ppu_write(const uint16_t addr, const uint8_t data);

    friend std::ostream& operator<<(std::ostream& os, const Cart& cart);

private:
    std::string m_filename;
    Header m_header;

    uint8_t m_num_prg_banks;
    std::vector<uint8_t> m_prg_mem;
    uint8_t m_num_chr_banks;
    std::vector<uint8_t> m_chr_mem;

    uint16_t m_mapper_id;
    std::shared_ptr<nes::cart::mapper::Mapper> m_mapper;

    inline void set_prg(const uint32_t addr, const uint8_t data) {
        if (addr >= m_prg_mem.size()) {
            m_prg_mem.resize(addr + 1);
        }
        m_prg_mem[addr] = data;
    }

    inline void set_chr(const uint32_t addr, const uint8_t data) {
        if (addr >= m_chr_mem.size()) {
            m_chr_mem.resize(addr + 1);
        }
        m_chr_mem[addr] = data;
    }
};

}} // nes::cart