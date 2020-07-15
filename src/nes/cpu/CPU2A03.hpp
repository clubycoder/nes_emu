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

#include <nes/Component.hpp>

// Forward declaration for Bus
namespace nes {

class Bus;

} // nes

namespace nes { namespace cpu {

class CPU2A03 : public Component {
public:
    void reset() override;

    void clock() override;

    void connect_bus(std::shared_ptr<nes::Bus> bus);
	
	void irq(); // Interrupt Request - Executes an instruction at a specific location
	void nmi(); // Non-Maskable Interrupt Request - As above, but cannot be disabled

    // Fill out Component requirements with stubs for CPU read/write
    const bool cpu_read(const uint16_t addr, uint8_t &data, const bool read_only = false) override { return false; };
    const bool cpu_write(const uint16_t addr, const uint8_t data) override { return false; };

private:
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
    } reg;

    enum StatusFlags 	{
        C = (1 << 0), // Carry
        Z = (1 << 1), // Zero
        I = (1 << 2), // Disable Interrupts
        B = (1 << 4), // Break
        V = (1 << 6), // Overflow
        N = (1 << 7) // Negative
    };

    bool get_status_flag(StatusFlags f);
    bool set_status_flag(StatusFlags f, bool value);

};

}} // nes::cpu