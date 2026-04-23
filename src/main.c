#include <stdio.h>
#include <stdlib.h>

#include "chip8.h"

Chip8 chip8;

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

    chip8_init(&chip8);

    printf("Loading ROM: '%s'\n", rom_file);
    chip8_load_rom(&chip8, rom_file);

    return 0;
}
