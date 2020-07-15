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
Emulation of ram
*******************************************************************************/

#include <cstdint>
#include <vector>
#include <algorithm>

#include <nes/ram/Ram.hpp>

namespace nes { namespace ram {

void Ram::reset() {
    m_data.resize(SIZE);
    std::fill(m_data.begin(), m_data.begin(), 0x00);
}

const bool Ram::cpu_read(const uint16_t addr, uint8_t &data, const bool read_only) {
    data = m_data[addr & (SIZE - 1)];
    return true;
}

const bool Ram::cpu_write(const uint16_t addr, const uint8_t data) {
    m_data[addr & (SIZE - 1)] = data;
    return true;
}

}} // nes::ram