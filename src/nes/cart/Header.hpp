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

#pragma once

#include <cstdint>

namespace nes { namespace cart {

const char MAGIC[] = {0x4E, 0x45, 0x53, 0x1A}; // NES followed by MS-DOS EOF

enum TimingType {
    NTSC = 0,
    PAL = 1,
    MULTI = 2,
    DENDY = 3
};

enum ExtDeviceType {
    UNSPECIFIED = 0x00, // Unspecified
    STANDARD_CONTROLLER = 0x01, // Standard NES/Famicom controllers
    FOUR_SCORE = 0x02, // NES Four Score/Satellite with two additional standard controllers
    FAMICOM_4_PLAYER = 0x03, // Famicom Four Players Adapter with two additional standard controllers
    VS = 0x04, // Vs. System
    VS_REVERSED = 0x05, // Vs. System with reversed inputs
    VS_PINBALL = 0x06, // Vs. Pinball (Japan)
    VS_ZAPPER = 0x07, // Vs. Zapper
    ZAPPER = 0x08, // Zapper ($4017)
    DUAL_ZAPPER = 0x09, // Two Zappers
    HYPER_SHOT_BANDAI = 0x0A, // Bandai Hyper Shot Lightgun
    POWER_PAD_A = 0x0B, // Power Pad Side A
    POWER_PAD_B = 0x0C, // Power Pad Side B
    FAMILY_TRAINER_A = 0x0D, // Family Trainer Side A
    FAMILY_TRAINER_B = 0x0E, // Family Trainer Side B
    ARKANOID_VAUS_NES = 0x0F, // Arkanoid Vaus Controller (NES)
    ARKANOID_VAUS_FAMICOM = 0x10, // Arkanoid Vaus Controller (Famicom)
    DUAL_VAUS_AND_FAMICOM_DATA_RECORDER = 0x11, // Two Vaus Controllers plus Famicom Data Recorder
    HYPER_SHOT_KONAMI = 0x12, // Konami Hyper Shot Controller
    COCONUTS_PACHINKO = 0x13, // Coconuts Pachinko Controller
    PUNCHING_BAG = 0x14, // Exciting Boxing Punching Bag (Blowup Doll)
    MAHJONG = 0x15, // Jissen Mahjong Controller
    PARTY_TAP = 0x16, // Party Tap 
    KIDS_TABLET = 0x17, // Oeka Kids Tablet
    BARCODE_BATTLER = 0x18, // Sunsoft Barcode Battler
    MIRACLE_PIANO = 0x19, // Miracle Piano Keyboard
    WHACK_A_MOLE = 0x1A, // Pokkun Moguraa (Whack-a-Mole Mat and Mallet)
    TOP_RIDER = 0x1B, // Top Rider (Inflatable Bicycle)
    DOUBLE_FISTED = 0x1C, // Double-Fisted (Requires or allows use of two controllers by one player)
    FAMICOM_3D = 0x1D, // Famicom 3D System
    DOREMIKKO_KEYBOARD = 0x1E , // Doremikko Keyboard
    ROB = 0x1F, // R.O.B. Gyro Set
    FAMICOM_DATA_RECORDER = 0x20, // Famicom Data Recorder (don't emulate keyboard)
    ASCII_TURBO_FILE = 0x21, // ASCII Turbo File
    IGS_STORAGE_BATTLE_BOX = 0x22, // IGS Storage Battle Box
    FAMILY_BASIC_KEYBOARD_AND_FAMICOM_DATA_RECORDER = 0x23, // Family BASIC Keyboard plus Famicom Data Recorder
    DONGDA_KEYBOARD = 0x24, // Dongda PEC-586 Keyboard
    BIT_CORP_KEYBOARD = 0x25, // Bit Corp. Bit-79 Keyboard
    SUBOR_KEYBOARD = 0x26, // Subor Keyboard
    SUBOR_KEYBOARD_AND_MOUSE_3x8 = 0x27, // Subor Keyboard plus mouse (3x8-bit protocol)
    SUBOR_KEYBOARD_AND_MOUSE_24 = 0x28, // Subor Keyboard plus mouse (24-bit protocol)
    SNES_MOUSE = 0x29, // SNES Mouse ($4017.d0)
    MULTICART = 0x2A, // Multicart
    DUAL_SNES_CONTROLLERS = 0x2B, // Two SNES controllers replacing the two standard NES controllers
    RACERMATE_BICYCLE = 0x2C, // RacerMate Bicycle
    U_FORCE = 0x2D, // U-Force
    ROB_STACKUP = 0x2E, // R.O.B. Stack-Up
    CITY_PATROLMAN_LIGHTBUN = 0x2F // City Patrolman Lightgun
};

#pragma pack(push, 1)
union Header {
    struct {
        char magic[4];
        uint8_t num_prg_banks;
        uint8_t num_chr_banks;
        union {
            struct {
                uint8_t mapper_id_low:4;
                bool ignore_mirroring:1;
                bool contains_trainer:1;
                bool contains_battery_backed_ram:1;
                bool mirror_vertical:1;
            };
            uint8_t bits;
        } flags6;
        union {
            struct {
                uint8_t mapper_id_high:4;
                uint8_t ines2:2;
                bool play_choice_10:1;
                bool vs_unisystem:1;
            };
            uint8_t bits;
        } flags7;
        uint8_t num_prg_ram_banks;
        union {
            struct {
                uint8_t unused1:7;
                bool tv_system_pal:1;
            };
            uint8_t bits;
        } flags9;
        union {
            struct {
                uint8_t unused1:2;
                bool has_bus_conflicts:1;
                bool no_prg_ram:1;
                uint8_t unused2:2;
                uint8_t tv_system:2;
            };
            uint8_t bits;
        } flags10;
        char unused[5];
    } ines1;
    struct {
        char magic[4];
        uint8_t num_prg_banks_low;
        uint8_t num_chr_banks_low;
        union {
            struct {
                uint8_t mapper_id_low:4;
                bool ignore_mirroring:1;
                bool contains_trainer:1;
                bool contains_battery_backed_ram:1;
                bool mirror_vertical:1;
            };
            uint8_t bits;
        } flags6;
        union {
            struct {
                uint8_t mapper_id_high:4;
                uint8_t ines2:2;
                bool console_type:2;
            };
            uint8_t bits;
        } flags7;
        union {
            struct {
                uint8_t mapper_id_extended:4;
                uint8_t sub_mapper_id:4;
            };
            uint8_t bits;
        } mapper;
        union {
            struct {
                uint8_t num_prg_banks_high:4;
                uint8_t num_chr_banks_high:4;
            };
            uint8_t bits;
        } rom;
        union {
            struct {
                uint8_t prg_ram_shift_count:4;
                uint8_t prg_flash_shift_count:4;
            };
            uint8_t bits;
        } prg_ram;
        union {
            struct {
                uint8_t chr_ram_shift_count:4;
                uint8_t chr_flash_shift_count:4;
            };
            uint8_t bits;
        } chr_ram;
        union {
            struct {
                uint8_t unused1:6;
                TimingType timing_type:2;
            };
            uint8_t bits;
        } timing;
        union {
            struct {
                uint8_t ppu_type:4;
                uint8_t hardware_type:4;
            };
            struct {
                uint8_t unused1:4;
                uint8_t extended_console_type:4;
            };
            uint8_t bits;
        } system;
        union {
            struct {
                uint8_t unused1:6;
                uint8_t num_misc_roms:2;
            };
            uint8_t bits;
        } mist_roms;
        union {
            struct {
                uint8_t unused1:2;
                ExtDeviceType default_exp_device_id:6;
            };
            uint8_t bits;
        } exp_device;
    } ines2;
};
#pragma pack(pop)

}} // nes::cart