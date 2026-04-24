#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"

#include "chip8.h"

#define TARGET_FPS 60
#define SCALE 16

#define BUFFER_SIZE 4096
#define SAMPLE_RATE 44100

float audio_buffer[4096] = { 0 };

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

    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(BUFFER_SIZE);
    AudioStream stream = LoadAudioStream(SAMPLE_RATE, 32, 1);
    SetAudioStreamVolume(stream, 0.5f);
    PlayAudioStream(stream);

    int sineFrequency = 440;
    int sineIndex = 0;

    while (!WindowShouldClose()) {
        chip8_update_keys(&chip8);
        for (int i = 0; i < CHIP8_CYCLES_PER_SECOND / TARGET_FPS; i++) {
            chip8_emulate_cycle(&chip8);
        }
        chip8_update_timers(&chip8, GetFrameTime());
        chip8_update_graphics(&chip8, SCALE);
        
        // Update audio

        if (IsAudioStreamProcessed(stream)) {
            for (int i = 0; i < BUFFER_SIZE; i++) {
                if (chip8.sound_timer > 0) {
                    float wavelength = (float)SAMPLE_RATE / sineFrequency;
                    audio_buffer[i] = sinf(2 * PI * sineIndex / wavelength);
                    sineIndex++;
                    if (sineIndex >= wavelength) {
                        sineIndex = 0;
                    }
                } else {
                    // Fill with silence if timer is 0
                    audio_buffer[i] = 0.0f;
                    sineIndex = 0;
                }
            }

            UpdateAudioStream(stream, audio_buffer, BUFFER_SIZE);
        }
    }

    UnloadAudioStream(stream);
    CloseAudioDevice();

    CloseWindow();

    return 0;
}
