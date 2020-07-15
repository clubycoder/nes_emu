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

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

#include <SDL2/SDL.h>

#include <nes/cart/Cart.hpp>

using namespace std;

const int NES_SCREEN_WIDTH = 256;
const int NES_SCREEN_HEIGHT = 240;
const int NES_SCREEN_SCALE = 3;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *screen = NULL;

int main(int argc, char *argv[]) {
    auto cart = argc > 1 ? nes::cart::Cart(argv[1]) : nes::cart::Cart("roms/donkey_kong.nes");
    std::cout << cart << std::endl;


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
	SDL_RenderSetLogicalSize(renderer, NES_SCREEN_WIDTH, NES_SCREEN_HEIGHT);

    screen = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        NES_SCREEN_WIDTH,
        NES_SCREEN_HEIGHT
    );
    if (!screen) {
        cerr << "ERROR: Unable to create screen texture!" << endl;
        return 3;
    }

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
            void *screen_pixels;
            int screen_pitch;
            if (SDL_LockTexture(screen, NULL, &screen_pixels, &screen_pitch) < 0) {
                cerr << "ERROR: Unable to lock screen texture!" << endl;
                return 4;
            }
            for (int y = 0; y < NES_SCREEN_HEIGHT; y++) {
                uint32_t *screen_row = (uint32_t *)((uint8_t *)screen_pixels + y * screen_pitch);
                for (int x = 0; x < NES_SCREEN_WIDTH; x++) {
                    uint8_t pixel_red = y;
                    uint8_t pixel_green = x;
                    uint8_t pixel_blue = y * x + pass;
                    *(screen_row + x) = (
                        0xFF000000L |
                        ((uint32_t)pixel_red & 0xFF) << 16 |
                        ((uint32_t)pixel_green & 0xFF) << 8 |
                        ((uint32_t)pixel_blue & 0xFF)
                    );
                }
            }
            pass++;
            SDL_UnlockTexture(screen);

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, screen, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
    }

    SDL_DestroyTexture(screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}