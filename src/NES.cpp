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

#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

#include <SDL2/SDL.h>

#include <nes/Bus.hpp>
#include <nes/cpu/CPU2A03.hpp>
#include <nes/ram/Ram.hpp>
#include <nes/ppu/PPU2C02SDL.hpp>
#include <nes/apu/APURP2A03SDL.hpp>
#include <nes/cart/Cart.hpp>

using namespace std;

const int NES_SCREEN_WIDTH = 256;
const int NES_SCREEN_HEIGHT = 240;
const int NES_SCREEN_SCALE = 3;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *screen = NULL;

int main(int argc, char *argv[]) {
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

    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "nearest"); 
	SDL_RenderSetLogicalSize(renderer, nes::ppu::PPU2C02::SCREEN_WIDTH, nes::ppu::PPU2C02::SCREEN_HEIGHT);

    auto cpu = std::make_shared<nes::cpu::CPU2A03>();
    auto ram = std::make_shared<nes::ram::Ram>();
    auto ppu = std::make_shared<nes::ppu::PPU2C02SDL>(renderer);
    auto apu = std::make_shared<nes::apu::APURP2A03SDL>();
    auto controller = std::make_shared<nes::controller::Controller>();

    auto bus = std::make_shared<nes::Bus>(cpu, ram, ppu, apu, controller);

    auto cart = std::make_shared<nes::cart::Cart>(argc > 1 ? argv[1] : "roms/donkey_kong.nes");
    std::cout << *cart << std::endl;

    bus->load_cart(cart);

    bus->reset();

    std::cout << *cpu << std::endl;

    bool done = false;
    int pass = 0;
    while (!done) {
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

        if (!done) {
            ppu->open_screen();
            for (int y = 0; y < NES_SCREEN_HEIGHT; y++) {
                for (int x = 0; x < NES_SCREEN_WIDTH; x++) {
                    uint8_t r = y;
                    uint8_t g = x;
                    uint8_t b = y * x + pass;
                    ppu->set_pixel(x, y, r, g, b);
                }
            }
            pass++;
            ppu->close_screen();

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, const_cast<SDL_Texture *>(ppu->get_screen_texture()), NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}