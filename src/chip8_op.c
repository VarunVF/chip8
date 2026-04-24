#include "chip8_op.h"

#include <assert.h>
#include <stdio.h>

// Macro for standard PC increment to keep stubs clean
#define NEXT chip8->PC += 2

// Mask for the last three nibbles (memory address)
#define NNN(opcode) ((opcode) & 0x0FFF)

// 0x0 Family

void chip8_op_clear_display(Chip8* chip8, uint16_t opcode)
{
    printf("CLS\n");
    NEXT;
}
void chip8_op_return(Chip8* chip8, uint16_t opcode)
{
    printf("RET\n");

    // Pop off the call stack
    assert(chip8->SP > 0);
    chip8->SP -= 1;								// point to the last saved address
    chip8->PC = chip8->stack[chip8->SP] + 2;	// move past the original call instruction
    chip8->stack[chip8->SP] = 0;				// clear the old address (for debugging)
    NEXT;
}

// Basic Flow

void chip8_op_jump(Chip8* chip8, uint16_t opcode)
{
    printf("JP %03X\n", NNN(opcode));
    chip8->PC = NNN(opcode);
}
void chip8_op_call(Chip8* chip8, uint16_t opcode)
{
    printf("CALL %03X\n", NNN(opcode));

    // Push onto the call stack
    assert(chip8->SP < CHIP8_STACK_SIZE);
    chip8->stack[chip8->SP] = chip8->PC;		// save return address
    chip8->SP += 1;
    chip8->PC = NNN(opcode);    				// jump to the function
    NEXT;
}
void chip8_op_skip_if_VX_eq_NN(Chip8* chip8, uint16_t opcode)
{
    printf("SE Vx, nn\n");
    NEXT;
}
void chip8_op_skip_if_VX_ne_NN(Chip8* chip8, uint16_t opcode)
{
    printf("SNE Vx, nn\n");
    NEXT;
}
void chip8_op_skip_if_VX_eq_VY(Chip8* chip8, uint16_t opcode)
{
    printf("SE Vx, Vy\n");
    NEXT;
}
void chip8_op_set_VX_to_NN(Chip8* chip8, uint16_t opcode)
{
    printf("LD Vx, nn\n");
    NEXT;
}
void chip8_op_add_NN_to_VX(Chip8* chip8, uint16_t opcode)
{
    printf("ADD Vx, nn\n");
    NEXT;
}

// 0x8 Math Family

void chip8_op_set_VX_to_VY(Chip8* chip8, uint16_t opcode)
{
    printf("LD Vx, Vy\n");
    NEXT;
}
void chip8_op_set_VX_to_VX_or_VY(Chip8* chip8, uint16_t opcode)
{
    printf("OR Vx, Vy\n");
    NEXT;
}
void chip8_op_set_VX_to_VX_and_VY(Chip8* chip8, uint16_t opcode)
{
    printf("AND Vx, Vy\n");
    NEXT;
}
void chip8_op_set_VX_to_VX_xor_VY(Chip8* chip8, uint16_t opcode)
{
    printf("XOR Vx, Vy\n");
    NEXT;
}
void chip8_op_add_VY_to_VX(Chip8* chip8, uint16_t opcode)
{
    printf("ADD Vx, Vy\n");
    NEXT;
}
void chip8_op_subtract_VY_from_VX(Chip8* chip8, uint16_t opcode)
{
    printf("SUB Vx, Vy\n");
    NEXT;
}
void chip8_op_right_shift_VX(Chip8* chip8, uint16_t opcode)
{
    printf("SHR Vx\n");
    NEXT;
}
void chip8_op_set_VX_to_VY_minus_VX(Chip8* chip8, uint16_t opcode)
{
    printf("SUBN Vx, Vy\n");
    NEXT;
}
void chip8_op_left_shift_VX(Chip8* chip8, uint16_t opcode)
{
    printf("SHL Vx\n");
    NEXT;
}

// Memory and Index

void chip8_op_skip_if_VX_ne_VY(Chip8* chip8, uint16_t opcode)
{
    printf("SNE Vx, Vy\n");
    NEXT;
}
void chip8_op_set_I_to_NNN(Chip8* chip8, uint16_t opcode)
{
    printf("LD I, %03X\n", NNN(opcode));
    NEXT;
}
void chip8_op_jump_to_NNN_plus_V0(Chip8* chip8, uint16_t opcode)
{
    printf("JP V0, addr\n");
    NEXT;
}
void chip8_op_set_VX_to_rand_and_NN(Chip8* chip8, uint16_t opcode)
{
    printf("RND Vx, nn\n");
    NEXT;
}
void chip8_op_draw(Chip8* chip8, uint16_t opcode)
{
    printf("DRW Vx, Vy, n\n");
    NEXT;
}

// Input and Timers

void chip8_op_skip_if_VX_key_pressed(Chip8* chip8, uint16_t opcode)
{
    printf("SKP Vx\n");
    NEXT;
}
void chip8_op_skip_if_VX_key_not_pressed(Chip8* chip8, uint16_t opcode)
{
    printf("SKNP Vx\n");
    NEXT;
}
void chip8_op_get_delay_timer_in_VX(Chip8* chip8, uint16_t opcode)
{
    printf("LD Vx, DT\n");
    NEXT;
}
void chip8_op_get_keypress_in_VX(Chip8* chip8, uint16_t opcode)
{
    printf("LD Vx, K\n");
    NEXT;
}
void chip8_op_set_delay_timer_to_VX(Chip8* chip8, uint16_t opcode)
{
    printf("LD DT, Vx\n");
    NEXT;
}
void chip8_op_set_sound_timer_to_VX(Chip8* chip8, uint16_t opcode)
{
    printf("LD ST, Vx\n");
    NEXT;
}
void chip8_op_add_VX_to_I(Chip8* chip8, uint16_t opcode)
{
    printf("ADD I, Vx\n");
    NEXT;
}
void chip8_op_set_I_to_sprite_address_in_VX(Chip8* chip8, uint16_t opcode)
{
    printf("LD F, Vx\n");
    NEXT;
}
void chip8_op_store_VX_as_BCD_at_address_in_I(Chip8* chip8, uint16_t opcode)
{
    printf("LD B, Vx\n");
    NEXT;
}
void chip8_op_reg_dump(Chip8* chip8, uint16_t opcode)
{
    printf("LD [I], Vx\n");
    NEXT;
}
void chip8_op_reg_load(Chip8* chip8, uint16_t opcode)
{
    printf("LD Vx, [I]\n");
    NEXT;
}
