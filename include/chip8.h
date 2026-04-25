#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#include "raylib.h"

// Code should execute starting from address 0x200,
// as the first 512 bytes are reserved for the interpreter.
#define CHIP8_ROM_OFFSET 0x200
#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_STACK_SIZE 256

#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_DISPLAY_SCALE 16
#define CHIP8_DISPLAY_TARGET_FPS 60

#define CHIP8_TIMER_FREQ 60.0
#define CHIP8_CYCLES_PER_SECOND 600

#define CHIP8_AUDIO_SAMPLE_RATE 44100
#define CHIP8_AUDIO_BUFFER_SIZE (CHIP8_AUDIO_SAMPLE_RATE / CHIP8_DISPLAY_TARGET_FPS)


enum Chip8SoundWaveType {
    CHIP8_SQUARE_WAVE,
    CHIP8_SINE_WAVE,
};

enum Chip8Status {
    CHIP8_OK = 1,
    CHIP8_ERR_ROM_FILE,
    CHIP8_ERR_ROM_SIZE,
};

typedef struct Chip8 {
    // Main memory
    uint8_t memory[CHIP8_MEMORY_SIZE];

    // Registers
    uint8_t V[16];                          // V registers (V0-VF)
    uint16_t PC;                            // Program counter (12 bits used)
    uint16_t I;                             // Index register (12 bits used)

    // Stack
    uint16_t stack[CHIP8_STACK_SIZE];       // Each return address uses 12 bits
    uint8_t SP;                             // Stack pointer

    // Timers
    uint8_t delay_timer;
    uint8_t sound_timer;
    double timer_accumulator;

    // Graphics
    uint8_t display[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT];

    // Input
    uint8_t keys[16];
    uint8_t is_awaiting_release;
    uint8_t waiting_key;

    // Sound
    AudioStream audio_stream;
    float audio_buffer[4096];
    int sine_frequency;
    int sine_index;
    enum Chip8SoundWaveType sound_wave_type;
} Chip8;

typedef void (*Chip8InstructionFunc)(Chip8* chip8, uint16_t opcode);

Chip8* chip8_init();
void chip8_free(Chip8* chip8);
int chip8_should_close(Chip8* chip8);
void chip8_set_sound_wave_type(Chip8* chip8, enum Chip8SoundWaveType type);
int chip8_load_rom(Chip8* chip8, const char* file_path);
void chip8_emulate_cycle(Chip8* chip8);
void chip8_emulate_frame(Chip8* chip8);
void chip8_update_timers(Chip8* chip8);
void chip8_update_graphics(Chip8* chip8);
void chip8_update_audio(Chip8* chip8);
void chip8_update_keys(Chip8* chip8);
Chip8InstructionFunc chip8_decode(Chip8* chip8, uint16_t opcode);

#endif // CHIP8_H
