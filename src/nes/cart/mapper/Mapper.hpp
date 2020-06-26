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
Emulation of the Mapper chip on the Nintendo Entertainment System Cartrige that
intercepts / adstracts the system, cartrige, and PPU memory access
*******************************************************************************/

#pragma once

#include <cstdint>

namespace nes { namespace cart { namespace mapper {

class Mapper {
public:
    Mapper(uint8_t numPRGBanks, uint8_t numCHRBanks)
        : m_numPRGBanks(numPRGBanks)
        , m_numCHRBanks(numCHRBanks) {
          reset();
    }
    ~Mapper();

    // Reset the mapper to a known state
    virtual void reset() = 0;

    // Manage read/write in CPU address space
    virtual bool cpuRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) = 0;
    virtual bool cpuWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data) = 0;

    // Manage read/write in the PPU address space
    virtual bool ppuRead(uint16_t addr, uint32_t &mapped_addr, uint8_t &data) = 0;
    virtual bool ppuWrite(uint16_t addr, uint32_t &mapped_addr, uint8_t data) = 0;

private:
    uint8_t m_numPRGBanks;
    uint8_t m_numCHRBanks;
};

}}} // nes::cart::mapper
