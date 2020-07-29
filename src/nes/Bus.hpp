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

#pragma once

#include <cstdint>
#include <memory>
#include <chrono>

#include <nes/Component.hpp>
#include <nes/cpu/CPU2A03.hpp>
#include <nes/ram/Ram.hpp>
#include <nes/ppu/PPU2C02.hpp>
#include <nes/apu/APURP2A03.hpp>
#include <nes/controller/Controller.hpp>
#include <nes/cart/Cart.hpp>

namespace nes {

class Bus : public Component {
public:
    Bus(
        std::shared_ptr<nes::cpu::CPU2A03> cpu,
        std::shared_ptr<nes::ram::Ram> ram,
        std::shared_ptr<nes::ppu::PPU2C02> ppu,
        std::shared_ptr<nes::apu::APURP2A03> apu,
        std::shared_ptr<nes::controller::Controller> controller
    )
        : m_cpu(cpu)
        , m_ram(ram)
        , m_ppu(ppu)
        , m_apu(apu)
        , m_controller(controller) {
    }

    void reset() override;

    void clock() override;

    void load_cart(std::shared_ptr<nes::cart::Cart> cart);

    const bool cpu_read(const uint16_t addr, uint8_t &data, const bool read_only = false) override;
    const bool cpu_write(const uint16_t addr, const uint8_t data) override;

private:
    static const uint16_t ADDR_RAM_BEGIN = 0x0000; static const uint16_t ADDR_RAM_END = 0x1FFF;
    static const uint16_t ADDR_PPU_BEGIN = 0x2000; static const uint16_t ADDR_PPU_END = 0x3FFF;
    static const uint16_t ADDR_APU_BEGIN = 0x4000; static const uint16_t ADDR_APU_END = 0x4013;
    static const uint16_t ADDR_APU_STATUS = 0x4015; static const uint16_t ADDR_APU_FRAME_COUNTER = 0x4017;
    static const uint16_t ADDR_DMA = 0x4014;
    static const uint16_t ADDR_CONTROLLER_BEGIN = 0x4016; static const uint16_t ADDR_CONTROLLER_END = 0x4017;

    std::shared_ptr<nes::cpu::CPU2A03> m_cpu;
    std::shared_ptr<nes::ram::Ram> m_ram;
    std::shared_ptr<nes::ppu::PPU2C02> m_ppu;
    std::shared_ptr<nes::apu::APURP2A03> m_apu;
    std::shared_ptr<nes::controller::Controller> m_controller;
    std::shared_ptr<nes::cart::Cart> m_cart;

    static constexpr uint32_t CLOCK_CHECK_AFTER_COUNT = 341 * 262; // Check roughly every screen render
    static constexpr double CLOCK_CHECK_AFTER_EXPECTED = ((1.0 / 60.0) * 1000.0); // Roughly 60 fps or 21441960Hz

    uint32_t m_clock_check_count;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_clock_check_last_timestamp;
};

} // nes