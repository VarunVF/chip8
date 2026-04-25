#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

void usage(void)
{
    fprintf(stderr, "Usage: chip8 <ROM_FILE>\n");
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        usage();
        return EXIT_FAILURE;
    }
    const char* rom_file = argv[1];

    Chip8* chip8 = chip8_init();
    if (chip8 == NULL) {
        fprintf(stderr, "Error: Failed to allocate a CHIP-8\n");
        return EXIT_FAILURE;
    }
    chip8_set_sound_wave_type(chip8, CHIP8_SQUARE_WAVE);

    printf("Loading ROM: '%s'\n", rom_file);
    int load_status = chip8_load_rom(chip8, rom_file);
    if (load_status == CHIP8_ERR_ROM_FILE) {
        fprintf(stderr, "Error: Could not open ROM file '%s'\n", rom_file);
        return EXIT_FAILURE;
    } else if (load_status == CHIP8_ERR_ROM_SIZE) {
        fprintf(stderr, "Error: ROM file '%s' is too large to fit in CHIP-8 memory\n", rom_file);
        return EXIT_FAILURE;
    }

    while (!chip8_should_close(chip8)) {
        chip8_update_keys(chip8);
        chip8_emulate_frame(chip8);
        chip8_update_timers(chip8);
        chip8_update_graphics(chip8);
        chip8_update_audio(chip8);
    }

    chip8_free(chip8);
    return 0;
}
