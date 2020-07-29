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

#pragma once

#include <cstdint>

#include <SDL2/SDL.h>

#include <nes/ppu/PPU2C02.hpp>

namespace nes { namespace ppu {

class PPU2C02SDL : public PPU2C02 {
public:
    PPU2C02SDL(SDL_Renderer *renderer)
        : PPU2C02()
        , m_renderer(renderer) {
        setup_screen_texture(renderer);
    };
    ~PPU2C02SDL();

    const SDL_Texture *get_screen_texture() const;

public: // TODO: Change to protected
    void open_screen() override;
    void close_screen() override;
    void set_pixel(const int x, const int y, const uint8_t r, const uint8_t g, const uint8_t b) override;

private:
    SDL_Renderer *m_renderer;
    SDL_Texture *m_screen_texture;
    int m_screen_pitch;
    void *m_screen_pixels;

    void setup_screen_texture(const SDL_Renderer *renderer);

};

}}