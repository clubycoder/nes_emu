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
Emulation of the addressing modes for the Ricoh 2A03

Links:
- http://www.oxyron.de/html/opcodes02.html
- https://wiki.nesdev.com/w/index.php/CPU_addressing_modes
- https://wiki.nesdev.com/w/index.php/CPU
- https://en.wikipedia.org/wiki/Ricoh_2A03
*******************************************************************************/

#include <cstdint>

#include <nes/cpu/CPU2A03Addressing.hpp>
#include <nes/cpu/CPU2A03.hpp>

namespace nes { namespace cpu {

/**
 * Implied, use value from accumulator
 */
bool CPU2A03Addressing::IMP(CPU2A03 &cpu) {
    cpu.m_instr_state.fetched = cpu.m_reg.a;
    return false;
}

/**
 * Immediate, grab from the next program counter address
 */
bool CPU2A03Addressing::IMM(CPU2A03 &cpu) {
    cpu.m_instr_state.addr_abs = cpu.m_reg.pc++;
    return false;
}

/**
 * Zero Page, grab the next byte from the program counter address as the address
 */
bool CPU2A03Addressing::ZP0(CPU2A03 &cpu) {
    cpu.m_instr_state.addr_abs = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    cpu.m_instr_state.addr_abs &= 0x00FF;
    return false;
}

/**
 * Zero Page + X, grab the next byte from the program counter address + X as the address
 */
bool CPU2A03Addressing::ZPX(CPU2A03 &cpu) {
    cpu.m_instr_state.addr_abs = cpu.bus_read(cpu.m_reg.pc) + cpu.m_reg.x;
    cpu.m_reg.pc++;
    cpu.m_instr_state.addr_abs &= 0x00FF;
    return false;
}

/**
 * Zero Page + Y, grab the next byte from the program counter address + Y as the address
 */
bool CPU2A03Addressing::ZPY(CPU2A03 &cpu) {
    cpu.m_instr_state.addr_abs = cpu.bus_read(cpu.m_reg.pc) + cpu.m_reg.y;
    cpu.m_reg.pc++;
    cpu.m_instr_state.addr_abs &= 0x00FF;
    return false;
}

/**
 * Relative, grab relative addtess within -128 and +128 from the program counter address
 */
bool CPU2A03Addressing::REL(CPU2A03 &cpu) {
    cpu.m_instr_state.addr_rel = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    // Fix for negative relative address
    if (cpu.m_instr_state.addr_rel & 0x80) {
        cpu.m_instr_state.addr_rel |= 0xFF00;
    }
    return false;
}

/**
 * Absolute, grab next 2 bytes to make a 16bit address
 */
bool CPU2A03Addressing::ABS(CPU2A03 &cpu) {
    uint16_t low = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    uint16_t high = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    cpu.m_instr_state.addr_abs = (high << 8) | low;
    return false;
}

/**
 * Absolute + X, grab next 2 bytes and add X to make a 16bit address.
 * Changing pages adds another clock cycle.
 */
bool CPU2A03Addressing::ABX(CPU2A03 &cpu) {
    uint16_t low = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    uint16_t high = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    cpu.m_instr_state.addr_abs = ((high << 8) | low) + cpu.m_reg.x;
    if ((cpu.m_instr_state.addr_abs & 0xFF00) != high << 8) {
        return true;
    }
    return false;
}

/**
 * Absolute + Y, grab next 2 bytes and add Y to make a 16bit address.
 * Changing pages adds another clock cycle.
 */
bool CPU2A03Addressing::ABY(CPU2A03 &cpu) {
    uint16_t low = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    uint16_t high = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    cpu.m_instr_state.addr_abs = ((high << 8) | low) + cpu.m_reg.y;
    if ((cpu.m_instr_state.addr_abs & 0xFF00) != high << 8) {
        return true;
    }
    return false;
}

/**
 * Indirect, grab next 2 bytes to make a 16bit address to grab the
 * actual 16bit address from.
 * Requires simulating a hardware bug when the low byte of the first
 * address is 0xFF to get it right.
 */
bool CPU2A03Addressing::IND(CPU2A03 &cpu) {
    uint16_t low = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    uint16_t high = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    uint16_t ptr = (high << 8) | low;
    // Simulate page boundary hardware bug
    if (low == 0x00FF) {
        cpu.m_instr_state.addr_abs = (cpu.bus_read(ptr & 0xFF00) << 8) | cpu.bus_read(ptr);
    } else {
        cpu.m_instr_state.addr_abs = (cpu.bus_read(ptr + 1) << 8) | cpu.bus_read(ptr);
    }
    return false;
}

/**
 * Indirect X, grab the next byte + X to get a pointer to the actual
 * 16bit address to use
 */
bool CPU2A03Addressing::IZX(CPU2A03 &cpu) {
    uint16_t ptr = cpu.bus_read(cpu.m_reg.pc) + cpu.m_reg.x;
    cpu.m_reg.pc++;
    cpu.m_instr_state.addr_abs = (cpu.bus_read((ptr + 1) & 0x00FF) << 8) | cpu.bus_read(ptr & 0x00FF);
    return false;
}

/**
 * Indirect Y, grab the next byteto get a pointer to the actual
 * 16bit address to use + Y.
 * Changing pages adds another clock cycle.
 */
bool CPU2A03Addressing::IZY(CPU2A03 &cpu) {
    uint16_t ptr = cpu.bus_read(cpu.m_reg.pc);
    cpu.m_reg.pc++;
    uint16_t low = cpu.bus_read(ptr & 0x00FF);
    uint16_t high = cpu.bus_read((ptr + 1) & 0x00FF);
    cpu.m_instr_state.addr_abs = ((high << 8) | low) + cpu.m_reg.y;
    if ((cpu.m_instr_state.addr_abs & 0xFF00) != high << 8) {
        return true;
    }
    return false;
}

}} // nes::cpu