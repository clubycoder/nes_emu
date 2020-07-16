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
Emulation of the instructions for the Ricoh 2A03

Links:
- http://www.oxyron.de/html/opcodes02.html
- https://wiki.nesdev.com/w/index.php/CPU
- https://en.wikipedia.org/wiki/Ricoh_2A03
*******************************************************************************/

#pragma once

#include <cstdint>

namespace nes { namespace cpu {

// Forward declaration to avoid cicular includes
class CPU2A03;

class CPU2A03Instructions {
public:
    static bool ADC(CPU2A03 &cpu);
    static bool AND(CPU2A03 &cpu);
    static bool ASL(CPU2A03 &cpu);
    static bool BCC(CPU2A03 &cpu);
    static bool BCS(CPU2A03 &cpu);
    static bool BEQ(CPU2A03 &cpu);
    static bool BIT(CPU2A03 &cpu);
    static bool BMI(CPU2A03 &cpu);
    static bool BNE(CPU2A03 &cpu);
    static bool BPL(CPU2A03 &cpu);
    static bool BRK(CPU2A03 &cpu);
    static bool BVC(CPU2A03 &cpu);
    static bool BVS(CPU2A03 &cpu);
    static bool CLC(CPU2A03 &cpu);
    static bool CLD(CPU2A03 &cpu);
    static bool CLI(CPU2A03 &cpu);
    static bool CLV(CPU2A03 &cpu);
    static bool CMP(CPU2A03 &cpu);
    static bool CPX(CPU2A03 &cpu);
    static bool CPY(CPU2A03 &cpu);
    static bool DEC(CPU2A03 &cpu);
    static bool DEX(CPU2A03 &cpu);
    static bool DEY(CPU2A03 &cpu);
    static bool EOR(CPU2A03 &cpu);
    static bool INC(CPU2A03 &cpu);
    static bool INX(CPU2A03 &cpu);
    static bool INY(CPU2A03 &cpu);
    static bool JMP(CPU2A03 &cpu);
    static bool JSR(CPU2A03 &cpu);
    static bool LDA(CPU2A03 &cpu);
    static bool LDX(CPU2A03 &cpu);
    static bool LDY(CPU2A03 &cpu);
    static bool LSR(CPU2A03 &cpu);
    static bool NOP(CPU2A03 &cpu);
    static bool ORA(CPU2A03 &cpu);
    static bool PHA(CPU2A03 &cpu);
    static bool PHP(CPU2A03 &cpu);
    static bool PLA(CPU2A03 &cpu);
    static bool PLP(CPU2A03 &cpu);
    static bool ROL(CPU2A03 &cpu);
    static bool ROR(CPU2A03 &cpu);
    static bool RTI(CPU2A03 &cpu);
    static bool RTS(CPU2A03 &cpu);
    static bool SBC(CPU2A03 &cpu);
    static bool SEC(CPU2A03 &cpu);
    static bool SED(CPU2A03 &cpu);
    static bool SEI(CPU2A03 &cpu);
    static bool STA(CPU2A03 &cpu);
    static bool STX(CPU2A03 &cpu);
    static bool STY(CPU2A03 &cpu);
    static bool TAX(CPU2A03 &cpu);
    static bool TAY(CPU2A03 &cpu);
    static bool TSX(CPU2A03 &cpu);
    static bool TXA(CPU2A03 &cpu);
    static bool TXS(CPU2A03 &cpu);
    static bool TYA(CPU2A03 &cpu);

	// Unofficial
	static bool XXX(CPU2A03 &cpu);
};

}} // nes::cpu