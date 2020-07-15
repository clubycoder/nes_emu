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
Emulation of the Ricoh 2A03 (modified MOS 6502) used as the CPU in the Nintendo
Entertainment System

Links:
- https://wiki.nesdev.com/w/index.php/CPU
- https://en.wikipedia.org/wiki/Ricoh_2A03
*******************************************************************************/

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstdint>

#include <utils/string_format.hpp>
#include <nes/cpu/CPU2A03.hpp>
#include <nes/Bus.hpp>

namespace nes { namespace cpu {

void CPU2A03::reset() {
}

void CPU2A03::clock() {
}

void CPU2A03::connect_bus(std::shared_ptr<nes::Bus> bus) {
    m_bus = bus;
}

uint8_t CPU2A03::bus_read(const uint16_t addr) {
    uint8_t data = 0x00;
    if (!m_bus->cpu_read(addr, data)) {
        throw std::runtime_error(utils::string_format("Invalid bus read from 0x%08X", addr));
    }
    return data;
}

void CPU2A03::bus_write(const uint16_t addr, const uint8_t data) {
    if (!m_bus->cpu_write(addr, data)) {
        throw std::runtime_error(utils::string_format("Invalid bus write to 0x%08X", addr));
    }
}

bool CPU2A03::get_status_flag(const StatusFlag f) const {
    return (m_reg.status & f) > 0;
}

void CPU2A03::set_status_flag(const StatusFlag f, const bool value) {
    if (value) {
        m_reg.status |= f;
    } else {
        m_reg.status &= ~f;
    }
}

std::ostream& operator<<(std::ostream& os, const CPU2A03& cpu) {
    os << utils::string_format("CPU-2A03: A=0x%02X(%d) X=0x%02X(%d) Y=0x%02X(%d) STKP=0x%02X PC=0x%04X STATUS=%02X",
        cpu.m_reg.a, cpu.m_reg.a, cpu.m_reg.x, cpu.m_reg.x, cpu.m_reg.y, cpu.m_reg.y,
        cpu.m_reg.stkp, cpu.m_reg.pc, cpu.m_reg.status) << std::endl;
    os << utils::string_format("  Status Flags: C=%d, Z=%d, I=%d, B=%d, V=%d, N=%d",
        cpu.get_status_flag(CPU2A03::StatusFlag::C),
        cpu.get_status_flag(CPU2A03::StatusFlag::Z),
        cpu.get_status_flag(CPU2A03::StatusFlag::I),
        cpu.get_status_flag(CPU2A03::StatusFlag::B),
        cpu.get_status_flag(CPU2A03::StatusFlag::O),
        cpu.get_status_flag(CPU2A03::StatusFlag::N)) << std::endl;

    return os;
}

}} // nes::cpu