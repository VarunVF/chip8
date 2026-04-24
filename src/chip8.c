#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "chip8_op.h"

void chip8_init(Chip8* chip8)
{
    if (chip8 == NULL) {
        return;
    }

    memset(chip8, 0, sizeof(chip8));
    // TODO: load fontset into first portion of memory

    chip8->PC = CHIP8_ROM_OFFSET;

    // seed rng
    srand(time(NULL));
}

void chip8_load_rom(Chip8* chip8, const char* file_path)
{
    // Open rom file to read bytes
    FILE* fp = fopen(file_path, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Could not read ROM file '%s'\n", file_path);
        return;
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
        fprintf(stderr, "ROM file '%s' is too large to fit in memory\n", file_path);
    }

    fclose(fp);
}

void chip8_emulate_cycle(Chip8* chip8)
{
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
        fprintf(stderr, "Unknown instruction: 0x%04X\n", opcode);
        exit(EXIT_FAILURE);
    }
}

Chip8InstructionFunc chip8_decode(Chip8* chip8, uint16_t opcode)
{
    uint8_t first_nibble  = (opcode & 0xF000) >> 12;
    uint8_t last_nibble   =  opcode & 0x000F;

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
