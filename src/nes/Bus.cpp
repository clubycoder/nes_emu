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
Emulation of the bus for the Nintendo Entertainment System that connects all of
the components
*******************************************************************************/

#include <nes/Bus.hpp>

namespace nes {

void Bus::reset() {
}

void Bus::clock() {
    // Clock the PPU
    m_ppu->clock();

    // Clock the APU
    m_apu->clock();

    // TODO: Changed to 1/3 of PPU
    m_cpu->clock();
}

void Bus::load_cart(std::shared_ptr<nes::cart::Cart> cart) {
    reset();
}

uint8_t Bus::cpu_read(const uint16_t addr) {
    uint8_t data = 0x00;

    // Give the cartridge / mapper the chance to handle the read
    if (!m_cart->cpu_read(addr, data)) {
        if (addr >= ADDR_RAM_BEGIN && addr <= ADDR_RAM_END) {
            data = m_ram->cpu_read(addr);
        } else if (addr <= ADDR_PPU_BEGIN && addr <= ADDR_PPU_END) {
            data = m_ppu->cpu_read(addr);
        } else if (addr == ADDR_APU_STATUS) {
            data = m_apu->cpu_read(addr);
        } else if (addr <= ADDR_CONTROLLER_BEGIN && addr <= ADDR_CONTROLLER_END) {
        }
    }

    return data;
}

void Bus::cpu_write(const uint16_t addr, const uint8_t data) {
    // Give the cartridge / mapper the chance to handle the write
    if (!m_cart->cpu_write(addr, data)) {
        if (addr >= ADDR_RAM_BEGIN && addr <= ADDR_RAM_END) {
            m_ram->cpu_write(addr, data);
        } else if (addr <= ADDR_PPU_BEGIN && addr <= ADDR_PPU_END) {
            m_ppu->cpu_write(addr, data);
        } else if ((addr <= ADDR_APU_BEGIN && addr <= ADDR_APU_END) || addr == ADDR_APU_STATUS || addr == ADDR_APU_FRAME_COUNTER) {
            m_apu->cpu_write(addr, data);
        } else if (addr <= ADDR_CONTROLLER_BEGIN && addr <= ADDR_CONTROLLER_END) {
        }
    }
}

}