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
    m_reg.pc = (bus_read(RESET_PC_ADDR) << 8) | bus_read(RESET_PC_ADDR + 1);
    m_reg.a = 0x00;
    m_reg.x = 0x00;
    m_reg.y = 0x00;
    m_reg.stkp = RESET_STKP_START;
    m_reg.status = 0x00;
    set_status_flag(U, true);

    m_instr_state.opcode = 0x00;
    m_instr_state.fetched = 0x00;
    m_instr_state.addr_abs = 0x0000;
    m_instr_state.addr_rel = 0x0000;
    m_instr_state.cycles = RESET_CYCLES;

    m_disasm.clear();
    m_disasm_pc_min = UINT32_MAX;
    m_disasm_pc_max = 0;
}

void CPU2A03::clock() {
    // make sure we have used up the current instruction's cycles before moving on to the next
    if (m_instr_state.cycles == 0) {
        std::cout << *this << std::endl;

        // Save off pc for disasm
        m_disasm_pc = m_reg.pc;

        // Read next opcode
        m_instr_state.opcode = bus_read(m_reg.pc);
        m_reg.pc++;

        // Unused status flag always needs to be set
        set_status_flag(U, true);

        // Lookup instruction
        m_instr_state.instruction = INT_LOOKUP[m_instr_state.opcode];

        // Set the base cycles
        m_instr_state.cycles = m_instr_state.instruction.cycles;

        // Generate addresses
        bool needs_additional_cycle1 = m_instr_state.instruction.evaluate_address(*this);

        // Disasm the instruction
        disasm_current();

        // Execute the instruction
        bool needs_additional_cycle2 = m_instr_state.instruction.execute(*this);

        // If both addressing and execution require another cycle, add one
        if (needs_additional_cycle1 && needs_additional_cycle2) {
            m_instr_state.cycles++;
        }

        // Unused status flag always needs to be set
        set_status_flag(U, true);
    }

    m_instr_state.cycles--;
}

void CPU2A03::connect_bus(std::shared_ptr<nes::Bus> bus) {
    m_bus = bus;
}

uint8_t CPU2A03::bus_read(const uint16_t addr) {
    uint8_t data = 0x00;
    if (!m_bus->cpu_read(addr, data)) {
        throw std::runtime_error(utils::string_format("Invalid bus read from 0x%08X", addr));
    }
    std::cout << utils::string_format("R: %02X", data) << std::endl;
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

void CPU2A03::disasm_current() {
    std::string addr_str = "";
    std::string addr_mode = "";
    if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::IMP) {
        addr_str = "";
        addr_mode = utils::string_format("IMP %02X", m_instr_state.addr_abs & 0x00FF);
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::IMM) {
        addr_str = utils::string_format("#$%02X", m_instr_state.addr_abs & 0x00FF);
        addr_mode = "IMM";
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::ZP0) {
        addr_str = utils::string_format("$%02X", m_instr_state.addr_abs & 0x00FF);
        addr_mode = "ZP0";
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::ZPX) {
        addr_str = utils::string_format("$%02X, X", m_instr_state.addr_abs & 0x00FF);
        addr_mode = "ZPX";
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::ZPY) {
        addr_str = utils::string_format("$%02X, Y", m_instr_state.addr_abs & 0x00FF);
        addr_mode = "ZPY";
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::REL) {
        addr_str = utils::string_format("$%02X", m_instr_state.addr_abs & 0x00FF);
        addr_mode = utils::string_format("REL %d", (int8_t)(m_instr_state.addr_abs & 0x00FF));
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::ABS) {
        addr_str = utils::string_format("$%04X", m_instr_state.addr_abs);
        addr_mode = "ABS";
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::ABX) {
        addr_str = utils::string_format("$%04X, X", m_instr_state.addr_abs);
        addr_mode = "ABX";
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::ABY) {
        addr_str = utils::string_format("$%04X, Y", m_instr_state.addr_abs);
        addr_mode = "ABY";
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::IND) {
        addr_str = utils::string_format("$%04X", m_instr_state.addr_abs);
        addr_mode = "IND";
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::IZX) {
        addr_str = utils::string_format("$%04X, X", m_instr_state.addr_abs);
        addr_mode = "IZX";
    } else if (m_instr_state.instruction.evaluate_address == CPU2A03Addressing::IZY) {
        addr_str = utils::string_format("$%04X, X", m_instr_state.addr_abs);
        addr_mode = "IZY";
    }
    std::string disasm = utils::string_format("$%04X: %s %s ; %s, %d cycles",
        m_disasm_pc, m_instr_state.instruction.name.c_str(),
        addr_str.c_str(), addr_mode.c_str(),
        m_instr_state.cycles);
    std::cout << disasm << std::endl;
    if (m_disasm_pc >= m_disasm.size()) {
        m_disasm.resize(m_disasm_pc + 1);
    }
    m_disasm[m_disasm_pc] = disasm;
    if (m_disasm_pc < m_disasm_pc_min) {
        m_disasm_pc_min = m_disasm_pc;
    }
    if (m_disasm_pc > m_disasm_pc_max) {
        m_disasm_pc_max = m_disasm_pc;
    }
}

std::ostream& operator<<(std::ostream& os, const CPU2A03& cpu) {
    os << utils::string_format("CPU-2A03: A=$%02X(%d) X=$%02X(%d) Y=$%02X(%d) STKP=$%02X PC=$%04X STATUS=$%02X",
        cpu.m_reg.a, cpu.m_reg.a, cpu.m_reg.x, cpu.m_reg.x, cpu.m_reg.y, cpu.m_reg.y,
        cpu.m_reg.stkp, cpu.m_reg.pc, cpu.m_reg.status);
    os << utils::string_format(" [C=%d, Z=%d, I=%d, B=%d, V=%d, N=%d]",
        cpu.get_status_flag(CPU2A03::StatusFlag::C),
        cpu.get_status_flag(CPU2A03::StatusFlag::Z),
        cpu.get_status_flag(CPU2A03::StatusFlag::I),
        cpu.get_status_flag(CPU2A03::StatusFlag::B),
        cpu.get_status_flag(CPU2A03::StatusFlag::O),
        cpu.get_status_flag(CPU2A03::StatusFlag::N));

    return os;
}

/**
 * 16x16 grid of instructions indexed by the opcode providing a name,
 * addressing & execution functions, and the base number of cycles to use.
 */
using I = CPU2A03Instructions;
using A = CPU2A03Addressing;
const std::vector<CPU2A03::Instruction> CPU2A03::INT_LOOKUP = {
    { "BRK", I::BRK, A::IMM, 7 },{ "ORA", I::ORA, A::IZX, 6 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "???", I::NOP, A::IMP, 3 },{ "ORA", I::ORA, A::ZP0, 3 },{ "ASL", I::ASL, A::ZP0, 5 },{ "???", I::XXX, A::IMP, 5 },{ "PHP", I::PHP, A::IMP, 3 },{ "ORA", I::ORA, A::IMM, 2 },{ "ASL", I::ASL, A::IMP, 2 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::NOP, A::IMP, 4 },{ "ORA", I::ORA, A::ABS, 4 },{ "ASL", I::ASL, A::ABS, 6 },{ "???", I::XXX, A::IMP, 6 },
    { "BPL", I::BPL, A::REL, 2 },{ "ORA", I::ORA, A::IZY, 5 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "???", I::NOP, A::IMP, 4 },{ "ORA", I::ORA, A::ZPX, 4 },{ "ASL", I::ASL, A::ZPX, 6 },{ "???", I::XXX, A::IMP, 6 },{ "CLC", I::CLC, A::IMP, 2 },{ "ORA", I::ORA, A::ABY, 4 },{ "???", I::NOP, A::IMP, 2 },{ "???", I::XXX, A::IMP, 7 },{ "???", I::NOP, A::IMP, 4 },{ "ORA", I::ORA, A::ABX, 4 },{ "ASL", I::ASL, A::ABX, 7 },{ "???", I::XXX, A::IMP, 7 },
    { "JSR", I::JSR, A::ABS, 6 },{ "AND", I::AND, A::IZX, 6 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "BIT", I::BIT, A::ZP0, 3 },{ "AND", I::AND, A::ZP0, 3 },{ "ROL", I::ROL, A::ZP0, 5 },{ "???", I::XXX, A::IMP, 5 },{ "PLP", I::PLP, A::IMP, 4 },{ "AND", I::AND, A::IMM, 2 },{ "ROL", I::ROL, A::IMP, 2 },{ "???", I::XXX, A::IMP, 2 },{ "BIT", I::BIT, A::ABS, 4 },{ "AND", I::AND, A::ABS, 4 },{ "ROL", I::ROL, A::ABS, 6 },{ "???", I::XXX, A::IMP, 6 },
    { "BMI", I::BMI, A::REL, 2 },{ "AND", I::AND, A::IZY, 5 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "???", I::NOP, A::IMP, 4 },{ "AND", I::AND, A::ZPX, 4 },{ "ROL", I::ROL, A::ZPX, 6 },{ "???", I::XXX, A::IMP, 6 },{ "SEC", I::SEC, A::IMP, 2 },{ "AND", I::AND, A::ABY, 4 },{ "???", I::NOP, A::IMP, 2 },{ "???", I::XXX, A::IMP, 7 },{ "???", I::NOP, A::IMP, 4 },{ "AND", I::AND, A::ABX, 4 },{ "ROL", I::ROL, A::ABX, 7 },{ "???", I::XXX, A::IMP, 7 },
    { "RTI", I::RTI, A::IMP, 6 },{ "EOR", I::EOR, A::IZX, 6 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "???", I::NOP, A::IMP, 3 },{ "EOR", I::EOR, A::ZP0, 3 },{ "LSR", I::LSR, A::ZP0, 5 },{ "???", I::XXX, A::IMP, 5 },{ "PHA", I::PHA, A::IMP, 3 },{ "EOR", I::EOR, A::IMM, 2 },{ "LSR", I::LSR, A::IMP, 2 },{ "???", I::XXX, A::IMP, 2 },{ "JMP", I::JMP, A::ABS, 3 },{ "EOR", I::EOR, A::ABS, 4 },{ "LSR", I::LSR, A::ABS, 6 },{ "???", I::XXX, A::IMP, 6 },
    { "BVC", I::BVC, A::REL, 2 },{ "EOR", I::EOR, A::IZY, 5 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "???", I::NOP, A::IMP, 4 },{ "EOR", I::EOR, A::ZPX, 4 },{ "LSR", I::LSR, A::ZPX, 6 },{ "???", I::XXX, A::IMP, 6 },{ "CLI", I::CLI, A::IMP, 2 },{ "EOR", I::EOR, A::ABY, 4 },{ "???", I::NOP, A::IMP, 2 },{ "???", I::XXX, A::IMP, 7 },{ "???", I::NOP, A::IMP, 4 },{ "EOR", I::EOR, A::ABX, 4 },{ "LSR", I::LSR, A::ABX, 7 },{ "???", I::XXX, A::IMP, 7 },
    { "RTS", I::RTS, A::IMP, 6 },{ "ADC", I::ADC, A::IZX, 6 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "???", I::NOP, A::IMP, 3 },{ "ADC", I::ADC, A::ZP0, 3 },{ "ROR", I::ROR, A::ZP0, 5 },{ "???", I::XXX, A::IMP, 5 },{ "PLA", I::PLA, A::IMP, 4 },{ "ADC", I::ADC, A::IMM, 2 },{ "ROR", I::ROR, A::IMP, 2 },{ "???", I::XXX, A::IMP, 2 },{ "JMP", I::JMP, A::IND, 5 },{ "ADC", I::ADC, A::ABS, 4 },{ "ROR", I::ROR, A::ABS, 6 },{ "???", I::XXX, A::IMP, 6 },
    { "BVS", I::BVS, A::REL, 2 },{ "ADC", I::ADC, A::IZY, 5 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "???", I::NOP, A::IMP, 4 },{ "ADC", I::ADC, A::ZPX, 4 },{ "ROR", I::ROR, A::ZPX, 6 },{ "???", I::XXX, A::IMP, 6 },{ "SEI", I::SEI, A::IMP, 2 },{ "ADC", I::ADC, A::ABY, 4 },{ "???", I::NOP, A::IMP, 2 },{ "???", I::XXX, A::IMP, 7 },{ "???", I::NOP, A::IMP, 4 },{ "ADC", I::ADC, A::ABX, 4 },{ "ROR", I::ROR, A::ABX, 7 },{ "???", I::XXX, A::IMP, 7 },
    { "???", I::NOP, A::IMP, 2 },{ "STA", I::STA, A::IZX, 6 },{ "???", I::NOP, A::IMP, 2 },{ "???", I::XXX, A::IMP, 6 },{ "STY", I::STY, A::ZP0, 3 },{ "STA", I::STA, A::ZP0, 3 },{ "STX", I::STX, A::ZP0, 3 },{ "???", I::XXX, A::IMP, 3 },{ "DEY", I::DEY, A::IMP, 2 },{ "???", I::NOP, A::IMP, 2 },{ "TXA", I::TXA, A::IMP, 2 },{ "???", I::XXX, A::IMP, 2 },{ "STY", I::STY, A::ABS, 4 },{ "STA", I::STA, A::ABS, 4 },{ "STX", I::STX, A::ABS, 4 },{ "???", I::XXX, A::IMP, 4 },
    { "BCC", I::BCC, A::REL, 2 },{ "STA", I::STA, A::IZY, 6 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 6 },{ "STY", I::STY, A::ZPX, 4 },{ "STA", I::STA, A::ZPX, 4 },{ "STX", I::STX, A::ZPY, 4 },{ "???", I::XXX, A::IMP, 4 },{ "TYA", I::TYA, A::IMP, 2 },{ "STA", I::STA, A::ABY, 5 },{ "TXS", I::TXS, A::IMP, 2 },{ "???", I::XXX, A::IMP, 5 },{ "???", I::NOP, A::IMP, 5 },{ "STA", I::STA, A::ABX, 5 },{ "???", I::XXX, A::IMP, 5 },{ "???", I::XXX, A::IMP, 5 },
    { "LDY", I::LDY, A::IMM, 2 },{ "LDA", I::LDA, A::IZX, 6 },{ "LDX", I::LDX, A::IMM, 2 },{ "???", I::XXX, A::IMP, 6 },{ "LDY", I::LDY, A::ZP0, 3 },{ "LDA", I::LDA, A::ZP0, 3 },{ "LDX", I::LDX, A::ZP0, 3 },{ "???", I::XXX, A::IMP, 3 },{ "TAY", I::TAY, A::IMP, 2 },{ "LDA", I::LDA, A::IMM, 2 },{ "TAX", I::TAX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 2 },{ "LDY", I::LDY, A::ABS, 4 },{ "LDA", I::LDA, A::ABS, 4 },{ "LDX", I::LDX, A::ABS, 4 },{ "???", I::XXX, A::IMP, 4 },
    { "BCS", I::BCS, A::REL, 2 },{ "LDA", I::LDA, A::IZY, 5 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 5 },{ "LDY", I::LDY, A::ZPX, 4 },{ "LDA", I::LDA, A::ZPX, 4 },{ "LDX", I::LDX, A::ZPY, 4 },{ "???", I::XXX, A::IMP, 4 },{ "CLV", I::CLV, A::IMP, 2 },{ "LDA", I::LDA, A::ABY, 4 },{ "TSX", I::TSX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 4 },{ "LDY", I::LDY, A::ABX, 4 },{ "LDA", I::LDA, A::ABX, 4 },{ "LDX", I::LDX, A::ABY, 4 },{ "???", I::XXX, A::IMP, 4 },
    { "CPY", I::CPY, A::IMM, 2 },{ "CMP", I::CMP, A::IZX, 6 },{ "???", I::NOP, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "CPY", I::CPY, A::ZP0, 3 },{ "CMP", I::CMP, A::ZP0, 3 },{ "DEC", I::DEC, A::ZP0, 5 },{ "???", I::XXX, A::IMP, 5 },{ "INY", I::INY, A::IMP, 2 },{ "CMP", I::CMP, A::IMM, 2 },{ "DEX", I::DEX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 2 },{ "CPY", I::CPY, A::ABS, 4 },{ "CMP", I::CMP, A::ABS, 4 },{ "DEC", I::DEC, A::ABS, 6 },{ "???", I::XXX, A::IMP, 6 },
    { "BNE", I::BNE, A::REL, 2 },{ "CMP", I::CMP, A::IZY, 5 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "???", I::NOP, A::IMP, 4 },{ "CMP", I::CMP, A::ZPX, 4 },{ "DEC", I::DEC, A::ZPX, 6 },{ "???", I::XXX, A::IMP, 6 },{ "CLD", I::CLD, A::IMP, 2 },{ "CMP", I::CMP, A::ABY, 4 },{ "NOP", I::NOP, A::IMP, 2 },{ "???", I::XXX, A::IMP, 7 },{ "???", I::NOP, A::IMP, 4 },{ "CMP", I::CMP, A::ABX, 4 },{ "DEC", I::DEC, A::ABX, 7 },{ "???", I::XXX, A::IMP, 7 },
    { "CPX", I::CPX, A::IMM, 2 },{ "SBC", I::SBC, A::IZX, 6 },{ "???", I::NOP, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "CPX", I::CPX, A::ZP0, 3 },{ "SBC", I::SBC, A::ZP0, 3 },{ "INC", I::INC, A::ZP0, 5 },{ "???", I::XXX, A::IMP, 5 },{ "INX", I::INX, A::IMP, 2 },{ "SBC", I::SBC, A::IMM, 2 },{ "NOP", I::NOP, A::IMP, 2 },{ "???", I::SBC, A::IMP, 2 },{ "CPX", I::CPX, A::ABS, 4 },{ "SBC", I::SBC, A::ABS, 4 },{ "INC", I::INC, A::ABS, 6 },{ "???", I::XXX, A::IMP, 6 },
    { "BEQ", I::BEQ, A::REL, 2 },{ "SBC", I::SBC, A::IZY, 5 },{ "???", I::XXX, A::IMP, 2 },{ "???", I::XXX, A::IMP, 8 },{ "???", I::NOP, A::IMP, 4 },{ "SBC", I::SBC, A::ZPX, 4 },{ "INC", I::INC, A::ZPX, 6 },{ "???", I::XXX, A::IMP, 6 },{ "SED", I::SED, A::IMP, 2 },{ "SBC", I::SBC, A::ABY, 4 },{ "NOP", I::NOP, A::IMP, 2 },{ "???", I::XXX, A::IMP, 7 },{ "???", I::NOP, A::IMP, 4 },{ "SBC", I::SBC, A::ABX, 4 },{ "INC", I::INC, A::ABX, 7 },{ "???", I::XXX, A::IMP, 7 }
};

}} // nes::cpu