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
SDL implementation of PPU
*******************************************************************************/

#include <stdexcept>
#include <cstdint>

#include <SDL2/SDL.h>

#include <nes/ppu/PPU2C02SDL.hpp>

namespace nes { namespace ppu {

void PPU2C02SDL::setup_screen_texture(SDL_Renderer *renderer) {
    m_screen_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
    if (!m_screen_texture) {
        throw std::runtime_error("Unable to create screen texture");
    }
}

const SDL_Texture *PPU2C02SDL::get_screen_texture() const {
    return m_screen_texture;
}

void PPU2C02SDL::open_screen() {
    if (SDL_LockTexture(m_screen_texture, NULL, &m_screen_pixels, &m_screen_pitch) < 0) {
        throw std::runtime_error("Unable to lock screen texture");
    }
}

void PPU2C02SDL::close_screen() {
    SDL_UnlockTexture(m_screen_texture);
}

void PPU2C02SDL::set_pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b) {
    uint32_t *screen_row = (uint32_t *)((uint8_t *)m_screen_pixels + y * m_screen_pitch);
    *(screen_row + x) = (
        0xFF000000L |
        ((uint32_t)r & 0xFF) << 16 |
        ((uint32_t)g & 0xFF) << 8 |
        ((uint32_t)b & 0xFF)
    );
}

}} // nes::ppu