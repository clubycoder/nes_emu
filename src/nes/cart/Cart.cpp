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
Emulation of the Nintendo Entertainment System Cartridge

Links:
- https://wiki.nesdev.com/w/index.php/INES
*******************************************************************************/

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>

#include <utils/string_format.hpp>
#include <nes/cart/Cart.hpp>
#include <nes/cart/mapper/Mapper000.hpp>

namespace nes { namespace cart {

//const char Cart::MAGIC[4] = {0x4E, 0x45, 0x53, 0x1A}; // NES followed by MS-DOS EOF

Cart::Cart(const std::string &filename) {
    m_filename = filename;

    std::ifstream ifs;
	ifs.open(m_filename, std::ifstream::binary);
	if (ifs.is_open()) {
		// Read header
		ifs.read((char *)&m_header, sizeof(Header));

        // Validate header
        if (strncmp(m_header.ines1.magic, MAGIC, 4) != 0) {
            throw std::runtime_error(utils::string_format("Cart magic is wrong.  Got %02X %02X %02X %02X but expected %02X %02X %02X %02X",
                m_header.ines1.magic[0], m_header.ines1.magic[1], m_header.ines1.magic[2], m_header.ines1.magic[3],
                MAGIC[0], MAGIC[1], MAGIC[2], MAGIC[0]
            ));
        }

        // Check for iNES2 style header
        if (m_header.ines1.flags7.ines2 == 2) {
            // Handle as iNES2
            throw std::runtime_error("iNES2 not supported");
        } else {
            // Handle as iNES1
            m_mapper_id = m_header.ines1.flags7.mapper_id_high << 4 | m_header.ines1.flags6.mapper_id_low;

            // Skip trainer if present
            if (m_header.ines1.flags6.contains_trainer) {
                ifs.seekg(512, std::ios_base::cur);
            }

            // Read the PRG image
            m_num_prg_banks = m_header.ines1.num_prg_banks;
            m_prg_mem.resize(m_num_prg_banks * PRG_BANK_SIZE);
            ifs.read((char*)m_prg_mem.data(), m_prg_mem.size());

            // Read the CHR image
            m_num_chr_banks = m_header.ines1.num_chr_banks;
            if (m_num_chr_banks == 0) {
                // Using RAM for CHR
                m_chr_mem.resize(CHR_BANK_SIZE);
            } else {
                // Using ROM for CHR
                m_chr_mem.resize(m_num_prg_banks * CHR_BANK_SIZE);
            }
            ifs.read((char*)m_chr_mem.data(), m_chr_mem.size());
        }

        // Setup the mapper
        switch (m_mapper_id) {
            case 0: m_mapper = std::make_shared<nes::cart::mapper::Mapper000>(m_num_prg_banks, m_num_chr_banks); break;
            default: throw std::runtime_error(utils::string_format("Mapper %u not supported", m_mapper_id));
        }
    }
}

Cart::~Cart() {
}

void Cart::reset() {

}

void Cart::clock() {
}

std::ostream& operator<<(std::ostream& os, const Cart& cart) {
    os << "NES Cartridge: " << cart.m_filename << std::endl;
    os << "  Magic: " << utils::string_format(
        "%02X %02X %02X %02X = %c%c%c",
        cart.m_header.ines1.magic[0], cart.m_header.ines1.magic[1], cart.m_header.ines1.magic[2], cart.m_header.ines1.magic[3],
        cart.m_header.ines1.magic[0], cart.m_header.ines1.magic[1], cart.m_header.ines1.magic[2]
    ) << ", Type: " << (cart.m_header.ines1.flags7.ines2 == 2 ? 2 : 1) << std::endl;
    os << "  Mapper " << cart.m_mapper_id << std::endl;
    if (cart.m_header.ines1.flags7.ines2 == 2) {
        // Display as iNES2
    } else {
        // Display as iNES1
    }
    return os;
}

}}