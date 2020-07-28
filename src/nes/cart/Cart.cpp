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
#include <nes/cart/mapper/Mapper999.hpp>
#include <nes/cpu/CPU2A03.hpp>

namespace nes { namespace cart {

//const char Cart::MAGIC[4] = {0x4E, 0x45, 0x53, 0x1A}; // NES followed by MS-DOS EOF

Cart::Cart(const std::string &filename) {
    // WARNING: This is a shared pointer hack to allow us to use shared_from_this()
    // from within the constructor so we can hand a shared pointer to the mapper.
    const auto trickDontRemove = std::shared_ptr<Cart>(this, [](Cart *){});

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

        setup_mapper();
    } else {
        throw std::runtime_error(utils::string_format("Failed to load ROM %s", m_filename.c_str()));
    }
}

Cart::Cart(const std::vector<uint8_t> &rom_memory, const uint16_t start_address) {
    // WARNING: This is a shared pointer hack to allow us to use shared_from_this()
    // from within the constructor so we can hand a shared pointer to the mapper.
    const auto trickDontRemove = std::shared_ptr<Cart>(this, [](Cart *){});

    std::cout << "HERE.A" << std::endl;

    m_filename = "<NONE>";

    memset(&m_header, 0, sizeof(Header));

    m_num_prg_banks = rom_memory.size() / PRG_BANK_SIZE;
    m_prg_mem = rom_memory;
    m_mapper_id = 999;

    setup_mapper();

    set_prg(nes::cpu::CPU2A03::RESET_PC_ADDR, start_address >> 8);
    set_prg(nes::cpu::CPU2A03::RESET_PC_ADDR + 1, start_address & 0xFF);
}

Cart::~Cart() {
}

void Cart::setup_mapper() {
    // Setup the mapper
    switch (m_mapper_id) {
        case 0: m_mapper = std::make_shared<nes::cart::mapper::Mapper000>(shared_from_this(), m_num_prg_banks, m_num_chr_banks); break;
        case 999: m_mapper = std::make_shared<nes::cart::mapper::Mapper999>(shared_from_this(), m_num_prg_banks, m_num_chr_banks); break;
        default: throw std::runtime_error(utils::string_format("Mapper %u not supported", m_mapper_id));
    }
}

void Cart::reset() {
    // Reset the mapper but do not reload the cartridge
    if (m_mapper != nullptr) {
        m_mapper->reset();
    }
}

const bool Cart::cpu_read(const uint16_t addr, uint8_t &data, const bool read_only) {
    data = 0x00;

    uint32_t mapped_addr = 0x00000000L;
    if (m_mapper->cpu_map_read_addr(addr, mapped_addr)) {
        if (mapped_addr < m_prg_mem.size()) {
            data = m_prg_mem[mapped_addr];
            return true;
        } else {
            throw std::runtime_error(utils::string_format("Mapped read address 0x%08X is out of PRG range 0x%08X - 0x%08X", mapped_addr, 0, m_prg_mem.size()));
        }
    }

    return false;
}

const bool Cart::cpu_write(const uint16_t addr, const uint8_t data) {
    uint32_t mapped_addr = 0x00000000L;
    if (m_mapper->cpu_map_write_addr(addr, mapped_addr, data)) {
        if (mapped_addr < m_prg_mem.size()) {
            m_prg_mem[mapped_addr] = data;
            return true;
        } else {
            throw std::runtime_error(utils::string_format("Mapped write address 0x%08X is out of PRG range 0x%08X - 0x%08X", mapped_addr, 0, m_prg_mem.size()));
        }
    }

    return false;
}

const bool Cart::ppu_read(const uint16_t addr, uint8_t &data, const bool read_only) {
    data = 0x00;

    uint32_t mapped_addr = 0x00000000L;
    if (m_mapper->ppu_map_read_addr(addr, mapped_addr)) {
        if (mapped_addr < m_chr_mem.size()) {
            data = m_chr_mem[mapped_addr];
            return true;
        } else {
            throw std::runtime_error(utils::string_format("Mapped read address 0x%08X is out of CHR range 0x%08X - 0x%08X", mapped_addr, 0, m_chr_mem.size()));
        }
    }

    return false;
}

const bool Cart::ppu_write(const uint16_t addr, const uint8_t data) {
    uint32_t mapped_addr = 0x00000000L;
    if (m_mapper->ppu_map_write_addr(addr, mapped_addr, data)) {
        if (mapped_addr < m_chr_mem.size()) {
            m_chr_mem[mapped_addr] = data;
            return true;
        } else {
            throw std::runtime_error(utils::string_format("Mapped write address 0x%08X is out of CHR range 0x%08X - 0x%08X", mapped_addr, 0, m_chr_mem.size()));
        }
    }

    return false;
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