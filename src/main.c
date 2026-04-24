#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#include "chip8.h"

#define TARGET_FPS 60
#define SCALE 16

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
    if (!chip8_load_rom(&chip8, rom_file)) {
        return EXIT_FAILURE;
    }

    InitWindow(64 * SCALE, 32 * SCALE, "CHIP-8");
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose()) {
        for (int i = 0; i < CHIP8_CYCLES_PER_SECOND / TARGET_FPS; i++) {
            chip8_emulate_cycle(&chip8);
        }
        chip8_update_timers(&chip8, GetFrameTime());

        BeginDrawing();
        ClearBackground(BLACK);
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                if (chip8.display[y * 64 + x]) {
                    DrawRectangle(x * SCALE, y * SCALE, SCALE, SCALE, WHITE);
                }
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
