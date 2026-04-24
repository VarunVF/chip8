#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

// Code should execute starting from address 0x200,
// as the first 512 bytes are reserved for the interpreter.
#define CHIP8_ROM_OFFSET 0x200
#define CHIP8_MEMORY_SIZE 4096
#define CHIP8_STACK_SIZE 256
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32

typedef struct Chip8 {
    uint8_t memory[CHIP8_MEMORY_SIZE];
    uint8_t V[16];                          // V registers (V0-VF)

    uint16_t PC;                            // Program counter (12 bits used)
    uint16_t I;                             // Index register (12 bits used)

    uint16_t stack[CHIP8_STACK_SIZE];       // Each return address uses 12 bits
    uint8_t SP;                             // Stack pointer

    // Timers
    uint8_t delay_timer;
    uint8_t sound_timer;

    // Graphics
    uint8_t display[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT];

    // TODO: Input, Sound
} Chip8;

typedef void (*Chip8InstructionFunc)(Chip8* chip8, uint16_t opcode);

void chip8_init(Chip8* chip8);
void chip8_load_rom(Chip8* chip8, const char* file_path);
void chip8_emulate_cycle(Chip8* chip8);
Chip8InstructionFunc chip8_decode(Chip8* chip8, uint16_t opcode);

#endif // CHIP8_H
