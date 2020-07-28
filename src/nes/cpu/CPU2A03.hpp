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

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <nes/Component.hpp>
#include <nes/cpu/CPU2A03Addressing.hpp>
#include <nes/cpu/CPU2A03Instructions.hpp>
#include <nes/cart/Cart.hpp>

// Forward declaration for Bus
namespace nes {

class Bus;

} // nes

namespace nes { namespace cpu {

class CPU2A03 : public Component {
public:
    void reset() override;

    void clock() override;

    void force_start_address(const uint16_t start_address);

    void connect_bus(std::shared_ptr<nes::Bus> bus);
	
	void irq(); // Interrupt Request - Executes an instruction at a specific location
	void nmi(); // Non-Maskable Interrupt Request - As above, but cannot be disabled

    // Fill out Component requirements with stubs for CPU read/write
    const bool cpu_read(const uint16_t addr, uint8_t &data, const bool read_only = false) override { return false; };
    const bool cpu_write(const uint16_t addr, const uint8_t data) override { return false; };

    friend std::ostream& operator<<(std::ostream& os, const CPU2A03& cpu);

private:
    friend class CPU2A03Addressing;
    friend class CPU2A03Instructions;

    // Cart has friend access just to reuse the consts below
    friend class nes::cart::Cart;

    static const uint16_t RESET_PC_ADDR = 0xFFFC;
    static const uint8_t RESET_STKP_START = 0xFD;
    static const uint8_t RESET_CYCLES = 8;

    int32_t m_start_address = -1;

    std::shared_ptr<nes::Bus> m_bus;
    uint8_t bus_read(const uint16_t addr);
    void bus_write(const uint16_t addr, const uint8_t data);

    struct Registers {
        uint8_t a = 0x00; // Accumulator
        uint8_t x = 0x00; // X
        uint8_t y = 0x00; // Y
        uint8_t stkp = 0x00; // Stack pointer (address on bus)
        uint16_t pc = 0x0000; // Program counter
        uint8_t status = 0x00; // Status
    } m_reg;

    enum StatusFlag {
        C = (1 << 0), // Carry
        Z = (1 << 1), // Zero
        I = (1 << 2), // Disable Interrupts
        B = (1 << 4), // Break
        U = (1 << 5), // Unused
        O = (1 << 6), // Overflow
        N = (1 << 7) // Negative
    };

    bool get_status_flag(const StatusFlag f) const;
    void set_status_flag(const StatusFlag f, const bool value);

    struct Instruction {
        std::string name;
        bool (*execute)(CPU2A03&) = nullptr;
        bool (*evaluate_address)(CPU2A03&) = nullptr;
        uint8_t cycles = 0;
    };
    static const std::vector<Instruction> INT_LOOKUP;

    struct InstructionState {
        uint8_t opcode;
        uint8_t cycles;
        uint8_t fetched;
        uint16_t addr_abs;
        uint16_t addr_rel;
        Instruction instruction;
    } m_instr_state;

    // Lines of disasembled instructions indexed by m_reg.pc
    std::vector<std::string> m_disasm;
    uint32_t m_disasm_pc, m_disasm_pc_min, m_disasm_pc_max;

    void disasm_current();
};

}} // nes::cpu