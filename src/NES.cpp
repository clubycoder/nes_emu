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
Nintendo Entertainment System Emulator

Links:
- https://wiki.nesdev.com/w/index.php/Nesdev
*******************************************************************************/

#include <stdexcept>
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>

#include <SDL2/SDL.h>

#include <nes/Bus.hpp>
#include <nes/cpu/CPU2A03.hpp>
#include <nes/ram/Ram.hpp>
#include <nes/ppu/PPU2C02SDL.hpp>
#include <nes/ppu/PPU2C02Headless.hpp>
#include <nes/apu/APURP2A03SDL.hpp>
#include <nes/apu/APURP2A03Headless.hpp>
#include <nes/cart/Cart.hpp>

using namespace std;

const int NES_SCREEN_WIDTH = 256;
const int NES_SCREEN_HEIGHT = 240;
const int NES_SCREEN_SCALE = 3;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *screen = NULL;

class Options {
public:
    Options(int argc, char **argv) {
        for (int argn = 1; argn < argc; argn += 2) {
            std::string key(argv[argn]);
            if (key == "-f") {
                std::string value(argv[argn + 1]);
                if (value.size() > 0) {
                    rom_filename = value;
                } else {
                    throw std::runtime_error("Filename must not be blank");
                }
            } else if (key == "-s") {
                /**
                 Test:
                   *=$8000
                   LDX #10
                   STX $0000
                   LDX #3
                   STX $0001
                   LDY $0000
                   LDA #0
                   CLC
                   loop
                   ADC $0001
                   DEY
                   BNE loop
                   STA $0002
                   NOP
                   NOP
                   NOP

                 Bytes:
                   A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA
                 */
                std::string value(argv[argn + 1]);
                if (value.size() > 0) {
                    std::stringstream rom_string;
                    rom_string << value;
                    rom.clear();
                    while (!rom_string.eof()) {
                        std::string byte_string;
                        rom_string >> byte_string;
                        rom.push_back((uint8_t)std::stoul(byte_string, nullptr, 16));
                    }
                } else {
                    throw std::runtime_error("Rom string must not be blank");
                }
            } else if (key == "-a") {
                std::string value(argv[argn + 1]);
                uint32_t pos = 0;
                if (value.size() > 1 && value.substr(0, 1) == "$") {
                    rom_start_address = std::stoul(value.substr(1), nullptr, 16);
                } else if (value.size() > 0) {
                    rom_start_address = std::stoul(value, nullptr, 16);
                } else {
                    throw std::runtime_error("Rom start address must not be blank");
                }
                if (rom_start_address < 0x0000 || rom_start_address > 0xFFFF) {
                    throw std::runtime_error("Rom start address must be between 0x0000 - 0xFFFF");
                }
            } else if (key == "-x") {
                headless = true;
                // Decrement the argement number because this argement doesn't take a value
                argn--;
            } else if (key == "-h") {
                std::cout << argv[0] << " (-f NES-ROM.nes | -s \"ROM BYTES\" -a $CODE-START)" << std::endl;
                std::cout << "  Start the Nintendo Entertainment System emulator with an NES ROM file:" << std::endl;
                std::cout << "    -f NES-ROM.nes - Path to iNES1 or iNES2 ROM file." << std::endl;
                std::cout << "  or with a string of bytes in hex separated by spaces that make up the rom:" << std::endl;
                std::cout << "    -s \"ROM BYTES\" - Space separated byte values in HEX that make up the rom." << std::endl;
                std::cout << "  Additional options:" << std::endl;
                std::cout << "    -h - Display this help." << std::endl;
                std::cout << "    -a $CODE-START - 16bit address for the start of code execution vs reading from 0xFFFC." << std::endl;
                std::cout << "    -x - Run headless (no graphics or sound) for debugging." << std::endl;
                exit(0);
            }
        }

        if (rom.size() > 0) {
            rom_filename.clear();
        } else if (rom_filename.size() <= 0) {
            rom_filename = "roms/nestest.nes";
        }
    }

    std::vector<uint8_t> rom;
    int32_t rom_start_address;
    std::string rom_filename;
    bool headless;
};

int main(int argc, char **argv) {
    Options options(argc, argv);

    if (!options.headless) {
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

        window = SDL_CreateWindow(
            "Nintendo Entertainment System",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            NES_SCREEN_WIDTH * NES_SCREEN_SCALE,
            NES_SCREEN_HEIGHT * NES_SCREEN_SCALE,
            SDL_WINDOW_SHOWN
        );
        if (!window) {
            cerr << "ERROR: Unable to create window!" << endl;
            return 1;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            cerr << "ERROR: Unable to create renderer!" << endl;
            return 2;
        }

        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest"); 
	    SDL_RenderSetLogicalSize(renderer, nes::ppu::PPU2C02::SCREEN_WIDTH, nes::ppu::PPU2C02::SCREEN_HEIGHT);
    }

    auto cpu = std::make_shared<nes::cpu::CPU2A03>();
    auto ram = std::make_shared<nes::ram::Ram>();
    auto ppu = (
        !options.headless ?
        (std::shared_ptr<nes::ppu::PPU2C02>)std::make_shared<nes::ppu::PPU2C02SDL>(renderer) :
        (std::shared_ptr<nes::ppu::PPU2C02>)std::make_shared<nes::ppu::PPU2C02Headless>()
    );
    auto apu = (
        !options.headless ?
        (std::shared_ptr<nes::apu::APURP2A03>)std::make_shared<nes::apu::APURP2A03SDL>() :
        (std::shared_ptr<nes::apu::APURP2A03>)std::make_shared<nes::apu::APURP2A03Headless>()
    );
    auto controller = std::make_shared<nes::controller::Controller>();

    auto bus = std::make_shared<nes::Bus>(cpu, ram, ppu, apu, controller);

    cpu->connect_bus(bus);

    auto cart = (
        options.rom_filename.size() > 0 ?
        std::make_shared<nes::cart::Cart>(options.rom_filename) :
        std::make_shared<nes::cart::Cart>(options.rom)
    );
    std::cout << *cart << std::endl;

    bus->load_cart(cart);

    if (options.rom_start_address > 0) {
        cpu->force_start_address((uint16_t)options.rom_start_address);
    }

    bus->reset();

    bool done = false;
    while (!done) {
        if (!options.headless) {
            SDL_Event event;
            while (SDL_PollEvent(&event) != 0) {
                switch (event.type) {
                    case SDL_KEYDOWN:
                        if (event.key.keysym.sym == SDLK_ESCAPE) {
                            done = true;
                        }
                        break;
                    case SDL_QUIT:
                        done = true;
                        break;
                }
		    }
        }

        if (!done) {
            bus->clock();
        }
    }

    if (!options.headless) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    return 0;
}