#include "chip8.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "raylib.h"

#include "chip8_op.h"

const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

// Array of physical keys in the order of CHIP-8 hex indices (0-F)
const int key_map[16] = {
    KEY_X,    // 0
    KEY_ONE,  // 1
    KEY_TWO,  // 2
    KEY_THREE,// 3
    KEY_Q,    // 4
    KEY_W,    // 5
    KEY_E,    // 6
    KEY_A,    // 7
    KEY_S,    // 8
    KEY_D,    // 9
    KEY_Z,    // A
    KEY_C,    // B
    KEY_FOUR, // C
    KEY_R,    // D
    KEY_F,    // E
    KEY_V     // F
};


Chip8* chip8_init()
{
    // Init CHIP-8
    Chip8* chip8 = malloc(sizeof(Chip8));
    if (chip8 == NULL) {
        return NULL;
    }
    memset(chip8, 0, sizeof(*chip8));

    // Init graphics window
    InitWindow(64 * CHIP8_DISPLAY_SCALE, 32 * CHIP8_DISPLAY_SCALE, "CHIP-8");
    SetTargetFPS(CHIP8_DISPLAY_TARGET_FPS);

    // Init PC to start of ROM
    chip8->PC = CHIP8_ROM_OFFSET;
    
    // Init fontset
    for (int i = 0; i < 80; i++) {
        chip8->memory[i] = fontset[i];
    }
    
    // Init audio
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(CHIP8_AUDIO_BUFFER_SIZE);
    chip8->audio_stream = LoadAudioStream(CHIP8_AUDIO_SAMPLE_RATE, 32, 1);
    SetAudioStreamVolume(chip8->audio_stream, 0.5f);
    PlayAudioStream(chip8->audio_stream);
    chip8->sound_wave_type = CHIP8_SQUARE_WAVE;
    chip8->sine_frequency = 440;
    chip8->sine_index = 0;

    // Seed rng (used for CXNN instruction)
    srand((unsigned int)time(NULL));

    return chip8;
}

void chip8_free(Chip8* chip8)
{
    // Free audio
    UnloadAudioStream(chip8->audio_stream);
    CloseAudioDevice();

    // Free window
    CloseWindow();

    // Free CHIP-8
    free(chip8);
}

int chip8_should_close(Chip8* chip8)
{
    return WindowShouldClose();
}

void chip8_set_sound_wave_type(Chip8* chip8, enum Chip8SoundWaveType type)
{
    chip8->sound_wave_type = type;
}

int chip8_load_rom(Chip8* chip8, const char* file_path)
{
    // Open rom file to read bytes
    FILE* fp = fopen(file_path, "rb");
    if (fp == NULL) {
        return CHIP8_ERR_ROM_FILE;
    }

    // Find file size
    fseek(fp, 0, SEEK_END);
    long rom_size = ftell(fp);
    rewind(fp);

    // Read into memory if the size fits
    if (rom_size < (CHIP8_MEMORY_SIZE - CHIP8_ROM_OFFSET)) {
        uint8_t* start = chip8->memory + CHIP8_ROM_OFFSET;
        fread(start, sizeof(uint8_t), rom_size, fp);
    }
    else {
        return CHIP8_ERR_ROM_SIZE;
    }

    fclose(fp);
    return CHIP8_OK;
}

void chip8_emulate_cycle(Chip8* chip8)
{
#ifndef NDEBUG
    printf("Instruction at 0x%04X: ", chip8->PC);
#endif

    // Fetch. Each opcode is two bytes long and stored in big-endian.
    uint8_t first_byte  = chip8->memory[chip8->PC];
    uint8_t second_byte = chip8->memory[chip8->PC + 1];
    uint16_t opcode = first_byte << 8 | second_byte;

    // Decode
    Chip8InstructionFunc func = chip8_decode(chip8, opcode);

    // Execute
    if (func) {
        func(chip8, opcode);
    } else {
#ifndef NDEBUG
        fprintf(stderr, "Unknown instruction: 0x%04X\n", opcode);
#endif
        exit(EXIT_FAILURE);
    }
}

void chip8_emulate_frame(Chip8* chip8)
{
    const int cycles_per_frame = CHIP8_CYCLES_PER_SECOND / CHIP8_DISPLAY_TARGET_FPS;
    for (int i = 0; i < cycles_per_frame; i++) {
        chip8_emulate_cycle(chip8);
    }
}

void chip8_update_timers(Chip8* chip8)
{
    chip8->timer_accumulator += GetFrameTime();

    if (chip8->timer_accumulator >= 1.0 / CHIP8_TIMER_FREQ) {
        // Sound and Delay timers count down at 60Hz.
        if (chip8->delay_timer > 0) {
            chip8->delay_timer--;
        }
        if (chip8->sound_timer > 0) {
            chip8->sound_timer--;
        }

        chip8->timer_accumulator -= 1.0 / CHIP8_TIMER_FREQ;
    }
}

void chip8_update_graphics(Chip8* chip8)
{
    int scale = CHIP8_DISPLAY_SCALE;

    BeginDrawing();
    ClearBackground(BLACK);
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (chip8->display[y * 64 + x]) {
                DrawRectangle(x * scale, y * scale, scale, scale, WHITE);
            }
        }
    }
    EndDrawing();
}

void chip8_update_audio(Chip8* chip8)
{
    while (IsAudioStreamProcessed(chip8->audio_stream)) {
        for (int i = 0; i < CHIP8_AUDIO_BUFFER_SIZE; i++) {
            if (chip8->sound_timer > 0) {
                float wavelength = (float)CHIP8_AUDIO_SAMPLE_RATE / chip8->sine_frequency;
                if (chip8->sound_wave_type == CHIP8_SQUARE_WAVE) {
                    chip8->audio_buffer[i] = sinf(2 * PI * chip8->sine_index / wavelength) > 0.0f ? 1.0f : -1.0f;
                } else if (chip8->sound_wave_type == CHIP8_SINE_WAVE) {
                    chip8->audio_buffer[i] = sinf(2 * PI * chip8->sine_index / wavelength);
                }

                chip8->sine_index++;
                if (chip8->sine_index >= wavelength) {
                    chip8->sine_index = 0;
                }
            }
            else {
                // Fill with silence if timer is 0
                chip8->audio_buffer[i] = 0.0f;
                chip8->sine_index = 0;
            }
        }

        UpdateAudioStream(chip8->audio_stream, chip8->audio_buffer, CHIP8_AUDIO_BUFFER_SIZE);
    }
}


void chip8_update_keys(Chip8* chip8) {
    for (int i = 0; i < 16; i++) {
        if (IsKeyDown(key_map[i])) {
            chip8->keys[i] = 1;
        } else {
            chip8->keys[i] = 0;
        }
    }
}

Chip8InstructionFunc chip8_decode(Chip8* chip8, uint16_t opcode)
{
    uint8_t first_nibble = (opcode & 0xF000) >> 12;
    uint8_t last_nibble  =  opcode & 0x000F;

    uint8_t second_byte = opcode & 0x00FF;

    switch (first_nibble) {
        case 0x0:
            if (opcode == 0x00E0) {
                return chip8_op_clear_display;
            } else if (opcode == 0x00EE) {
                return chip8_op_return;
            } else {
                // 0x0NNN: Calls RCA 1802 machine code routine. Not implemented.
                return NULL;
            }
        case 0x1:
            return chip8_op_jump;
        case 0x2:
            return chip8_op_call;
        case 0x3:
            return chip8_op_skip_if_Vx_eq_NN;
        case 0x4:
            return chip8_op_skip_if_Vx_ne_NN;
        case 0x5:
            return chip8_op_skip_if_Vx_eq_Vy;
        case 0x6:
            return chip8_op_set_Vx_to_NN;
        case 0x7:
            return chip8_op_add_NN_to_Vx;
        case 0x8:
            switch (last_nibble) {
            case 0x0: return chip8_op_set_Vx_to_Vy;
            case 0x1: return chip8_op_set_Vx_to_Vx_or_Vy;
            case 0x2: return chip8_op_set_Vx_to_Vx_and_Vy;
            case 0x3: return chip8_op_set_Vx_to_Vx_xor_Vy;
            case 0x4: return chip8_op_add_Vy_to_Vx;
            case 0x5: return chip8_op_subtract_Vy_from_Vx;
            case 0x6: return chip8_op_right_shift_Vx;
            case 0x7: return chip8_op_set_Vx_to_Vy_minus_Vx;
            case 0xE: return chip8_op_left_shift_Vx;
            default:  return NULL;
            }
        case 0x9:
            return chip8_op_skip_if_Vx_ne_Vy;
        case 0xA:
            return chip8_op_set_I_to_NNN;
        case 0xB:
            return chip8_op_jump_to_NNN_plus_V0;
        case 0xC:
            return chip8_op_set_Vx_to_rand_and_NN;
        case 0xD:
            return chip8_op_draw;
        case 0xE:
            switch (second_byte) {
            case 0x9E: return chip8_op_skip_if_Vx_key_pressed;
            case 0xA1: return chip8_op_skip_if_Vx_key_not_pressed;
            default: return NULL;
            }
        case 0xF:
            switch (second_byte) {
            case 0x07: return chip8_op_get_delay_timer_in_Vx;
            case 0x0A: return chip8_op_get_keypress_in_Vx;
            case 0x15: return chip8_op_set_delay_timer_to_Vx;
            case 0x18: return chip8_op_set_sound_timer_to_Vx;
            case 0x1E: return chip8_op_add_Vx_to_I;
            case 0x29: return chip8_op_set_I_to_sprite_address_in_Vx;
            case 0x33: return chip8_op_store_Vx_as_BCD_at_address_in_I;
            case 0x55: return chip8_op_reg_dump;
            case 0x65: return chip8_op_reg_load;
            }
        default:
            return NULL;
    }

    return NULL;
}
