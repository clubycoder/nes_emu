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

#pragma once

#include <cstdint>

namespace nes { namespace cpu {

// Forward declaration to avoid cicular includes
class CPU2A03;

class CPU2A03Addressing {
public:
    static bool IMP(CPU2A03 &cpu);
    static bool IMM(CPU2A03 &cpu);
    static bool ZP0(CPU2A03 &cpu);
    static bool ZPX(CPU2A03 &cpu);
    static bool ZPY(CPU2A03 &cpu);
    static bool REL(CPU2A03 &cpu);
    static bool ABS(CPU2A03 &cpu);
    static bool ABX(CPU2A03 &cpu);
    static bool ABY(CPU2A03 &cpu);
    static bool IND(CPU2A03 &cpu);
    static bool IZX(CPU2A03 &cpu);
    static bool IZY(CPU2A03 &cpu);
};

}} // nes::cpu