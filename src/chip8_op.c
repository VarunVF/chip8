#include "chip8_op.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Macro for standard PC increment
#define NEXT chip8->PC += 2

// Mask for the last three nibbles (memory address)
#define NNN(opcode) ((opcode) & 0x0FFF)

// Mask for the second byte
#define NN(opcode) ((opcode) & 0x00FF)

// Mask for the last nibble
#define N(opcode) ((opcode) & 0x000F)

// Mask for the second nibble
#define X(opcode) (((opcode) & 0x0F00) >> 8)

// Mask for the third nibble
#define Y(opcode) (((opcode) & 0x00F0) >> 4)


// 0x0 Family

void chip8_op_clear_display(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("CLS\n");
#endif

    memset(chip8->display, 0, sizeof(chip8->display));
    NEXT;
}
void chip8_op_return(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("RET\n");
#endif

    // Pop off the call stack
    assert(chip8->SP > 0);
    chip8->SP -= 1;								// point to the last saved address
    chip8->PC = chip8->stack[chip8->SP] + 2;	// move past the original call instruction
    chip8->stack[chip8->SP] = 0;				// clear the old address (for debugging)
}

// Basic Flow

void chip8_op_jump(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("JP 0x%03X\n", NNN(opcode));
#endif

    chip8->PC = NNN(opcode);
}
void chip8_op_call(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("CALL 0x%03X\n", NNN(opcode));
#endif

    // Push onto the call stack
    assert(chip8->SP < CHIP8_STACK_SIZE);
    chip8->stack[chip8->SP] = chip8->PC;		// save return address
    chip8->SP += 1;
    chip8->PC = NNN(opcode);    				// jump to the function
}
void chip8_op_skip_if_Vx_eq_NN(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("SE Vx, 0x%02X\n", NN(opcode));
#endif

    uint8_t x = X(opcode);
    uint8_t nn = NN(opcode);
    if (chip8->V[x] == nn) {
        NEXT;                                   // Skip the next instruction
    }

    NEXT;
}
void chip8_op_skip_if_Vx_ne_NN(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("SNE Vx, 0x%02X\n", NN(opcode));
#endif
    
    uint8_t x = X(opcode);
    uint8_t nn = NN(opcode);
    if (chip8->V[x] != nn) {
        NEXT;                                   // Skip the next instruction
    }
    
    NEXT;
}
void chip8_op_skip_if_Vx_eq_Vy(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("SE Vx, Vy\n");
#endif

    uint8_t x = X(opcode);
    uint8_t y = Y(opcode);
    if (chip8->V[x] == chip8->V[y]) {
        NEXT;                                   // Skip the next instruction
    }

    NEXT;
}
void chip8_op_set_Vx_to_NN(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LD Vx, 0x%02X\n", NN(opcode));
#endif

    uint8_t x = X(opcode);
    chip8->V[x] = NN(opcode);
    NEXT;
}
void chip8_op_add_NN_to_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("ADD Vx, 0x%02X\n", NN(opcode));
#endif

    uint8_t x = X(opcode);
    chip8->V[x] += NN(opcode);
    NEXT;
}

// 0x8 Math Family

void chip8_op_set_Vx_to_Vy(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LD Vx, Vy\n");
#endif

    uint8_t x = X(opcode);
    uint8_t y = Y(opcode);
    chip8->V[x] = chip8->V[y];
    NEXT;
}
void chip8_op_set_Vx_to_Vx_or_Vy(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("OR Vx, Vy\n");
#endif

    uint8_t x = X(opcode);
    uint8_t y = Y(opcode);
    chip8->V[x] |= chip8->V[y];
    NEXT;
}
void chip8_op_set_Vx_to_Vx_and_Vy(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("AND Vx, Vy\n");
#endif

    uint8_t x = X(opcode);
    uint8_t y = Y(opcode);
    chip8->V[x] &= chip8->V[y];
    NEXT;
}
void chip8_op_set_Vx_to_Vx_xor_Vy(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("XOR Vx, Vy\n");
#endif

    uint8_t x = X(opcode);
    uint8_t y = Y(opcode);
    chip8->V[x] ^= chip8->V[y];
    NEXT;
}
void chip8_op_add_Vy_to_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("ADD Vx, Vy\n");
#endif

    // VF is set to 1 when there is overflow, and 0 when there is not.
    uint8_t x = X(opcode);
    uint8_t y = Y(opcode);
    uint16_t sum = chip8->V[x] + chip8->V[y];

    // Perform the addition first
    chip8->V[x] += chip8->V[y];

    // Set VF
    if (sum > 255) {
        chip8->V[0xF] = 1;
    } else {
        chip8->V[0xF] = 0;
    }

    NEXT;
}
void chip8_op_subtract_Vy_from_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("SUB Vx, Vy\n");
#endif

    // VF is set to 0 when there is underflow, and 1 when there is not.
    // i.e. VF set to 1 if VX >= VY and 0 if not.
    uint8_t x = X(opcode);
    uint8_t y = Y(opcode);
    uint8_t should_set_VF = chip8->V[x] >= chip8->V[y];

    // Perform the subtraction first
    chip8->V[x] -= chip8->V[y];

    // Set VF
    if (should_set_VF) {
        chip8->V[0xF] = 1;
    } else {
        chip8->V[0xF] = 0;
    }

    NEXT;
}
void chip8_op_right_shift_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("SHR Vx\n");
#endif

    // Store the bit that is about to be 'dropped'
    uint8_t x = X(opcode);
    uint8_t dropped = chip8->V[x] & 1;            // LSB of Vx

    // Perform the shift
    chip8->V[x] >>= 1;

    // Set VF
    chip8->V[0xF] = dropped;

    NEXT;
}
void chip8_op_set_Vx_to_Vy_minus_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("SUBN Vx, Vy\n");
#endif

    // VF set to 1 if VY >= VX, or 0 otherwise.
    uint8_t x = X(opcode);
    uint8_t y = Y(opcode);
    uint8_t should_set_VF = chip8->V[y] >= chip8->V[x];


    // Perform the subtraction first
    chip8->V[x] = chip8->V[y] - chip8->V[x];

    // Set VF
    if (should_set_VF) {
        chip8->V[0xF] = 1;
    }
    else {
        chip8->V[0xF] = 0;
    }

    NEXT;
}
void chip8_op_left_shift_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("SHL Vx\n");
#endif

    // Store the bit that is about to be 'dropped' in VF before shifting
    uint8_t x = X(opcode);
    uint8_t dropped = chip8->V[x] & 0b1000'0000;          // MSB of Vx

    // Perform the shift
    chip8->V[x] <<= 1;

    // Set VF
    chip8->V[0xF] = dropped >> 7;

    NEXT;
}

// Memory and Index

void chip8_op_skip_if_Vx_ne_Vy(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("SNE Vx, Vy\n");
#endif

    uint8_t x = X(opcode);
    uint8_t y = Y(opcode);
    if (chip8->V[x] != chip8->V[y]) {
        NEXT;                                   // Skip the next instruction
    }

    NEXT;
}
void chip8_op_set_I_to_NNN(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LD I, 0x%03X\n", NNN(opcode));
#endif

    chip8->I = NNN(opcode);
    NEXT;
}
void chip8_op_jump_to_NNN_plus_V0(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("JP V0, 0x%03X\n", NNN(opcode));
#endif

    chip8->PC = NNN(opcode) + chip8->V[0];
    NEXT;
}
void chip8_op_set_Vx_to_rand_and_NN(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("RAND Vx, 0x%02X\n", NN(opcode));
#endif

    uint8_t x = X(opcode);
    uint8_t random_byte = rand() % 256;
    chip8->V[x] = random_byte & NN(opcode);
    NEXT;
}
void chip8_op_draw(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("DRAW Vx, Vy, 0x%1X\n", N(opcode));
#endif

    // Draw a sprite at coordinate (Vx, Vy) with a width of 8 px and a height of N px.
    // Each row of 8 pixels is read as bit-coded starting from memory location I.
    // VF is set to 1 if any screen pixels were flipped from set to unset.
    // VF is set to 0 otherwise.

    uint8_t x = chip8->V[X(opcode)] % 64;
    uint8_t y = chip8->V[Y(opcode)] % 32;
    uint8_t height = N(opcode);

    chip8->V[0xF] = 0; // Reset collision flag

    for (int row = 0; row < height; row++) {
        uint8_t sprite_byte = chip8->memory[chip8->I + row];
        for (int col = 0; col < 8; col++) {
            if ((sprite_byte & (0x80 >> col)) != 0) {
                // Modulo ensures pixels wrap around the screen
                int screen_idx = ((y + row) % 32) * 64 + ((x + col) % 64);

                // Collision check
                if (chip8->display[screen_idx] == 1) {
                    chip8->V[0xF] = 1;
                }

                // XOR the display pixel (allows transparency and erasing)
                chip8->display[screen_idx] ^= 1;
            }
        }
    }

    NEXT;
}

// Input and Timers

void chip8_op_skip_if_Vx_key_pressed(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("SKP Vx\n");
#endif

    uint8_t key = chip8->V[X(opcode)];
    if (chip8->keys[key]) {
        NEXT;                                   // Skip the next instruction
    }
    NEXT;
}
void chip8_op_skip_if_Vx_key_not_pressed(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("SKNP Vx\n");
#endif

    uint8_t key = chip8->V[X(opcode)];
    if (!chip8->keys[key]) {
        NEXT;                                   // Skip the next instruction
    }
    NEXT;
}
void chip8_op_get_delay_timer_in_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LD Vx, DT\n");
#endif

    chip8->V[X(opcode)] = chip8->delay_timer;
    NEXT;
}
void chip8_op_get_keypress_in_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LD Vx, K\n");
#endif

    // This instruction blocks until a key is pressed.

    int key_pressed = 0;

    for (int i = 0; i < 16; i++) {
        if (chip8->keys[i]) {
            chip8->V[X(opcode)] = i;
            key_pressed = 1;
            break;
        }
    }

    // If no key is down, return without calling NEXT.
    // The PC stays on this opcode and the CPU runs it again next cycle.
    if (!key_pressed) {
        return;
    }

    NEXT;
}
void chip8_op_set_delay_timer_to_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LD DT, Vx\n");
#endif

    chip8->delay_timer = chip8->V[X(opcode)];
    NEXT;
}
void chip8_op_set_sound_timer_to_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LD ST, Vx\n");
#endif

    chip8->sound_timer = chip8->V[X(opcode)];
    NEXT;
}
void chip8_op_add_Vx_to_I(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("ADD I, Vx\n");
#endif

    chip8->I += chip8->V[X(opcode)];
    NEXT;
}
void chip8_op_set_I_to_sprite_address_in_Vx(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LDSPR, Vx\n");
#endif

    // Ensure each character is in 0x0 to 0xF
    uint8_t character = chip8->V[X(opcode)];
    chip8->I = (character & 0x0F) * 5;
    NEXT;
}
void chip8_op_store_Vx_as_BCD_at_address_in_I(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LDBCD Vx\n");
#endif

    // Store the binary-coded decimal representation of Vx
    uint8_t x = X(opcode);
    uint8_t value = chip8->V[x];
    chip8->memory[chip8->I] = value / 100;              // Hundreds
    chip8->memory[chip8->I + 1] = (value / 10) % 10;    // Tens
    chip8->memory[chip8->I + 2] = value % 10;           // Ones

    NEXT;
}
void chip8_op_reg_dump(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LD [I], Vx\n");
#endif

    // Stores from V0 to VX (including VX) in memory, starting at address I
    uint8_t x = X(opcode);
    for (uint8_t i = 0; i <= x; i++) {
        // I is left unmodified
        chip8->memory[chip8->I + i] = chip8->V[i];
    }
    
    NEXT;
}
void chip8_op_reg_load(Chip8* chip8, uint16_t opcode)
{
#ifndef NDEBUG
    printf("LD Vx, [I]\n");
#endif

    // Fills from V0 to VX (including VX) with values from memory, starting at address I
    uint8_t x = X(opcode);
    for (uint8_t i = 0; i <= x; i++) {
        // I is left unmodified
        chip8->V[i] = chip8->memory[chip8->I + i];
    }

    NEXT;
}
