#include "chip8.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
