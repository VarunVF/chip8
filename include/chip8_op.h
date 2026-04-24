#ifndef CHIP8_OP_H
#define CHIP8_OP_H

#include "chip8.h"

// 0x0 Family

void chip8_op_clear_display(Chip8* chip8, uint16_t opcode);
void chip8_op_return(Chip8* chip8, uint16_t opcode);

// Basic Flow

void chip8_op_jump(Chip8* chip8, uint16_t opcode);
void chip8_op_call(Chip8* chip8, uint16_t opcode);
void chip8_op_skip_if_Vx_eq_NN(Chip8* chip8, uint16_t opcode);
void chip8_op_skip_if_Vx_ne_NN(Chip8* chip8, uint16_t opcode);
void chip8_op_skip_if_Vx_eq_Vy(Chip8* chip8, uint16_t opcode);
void chip8_op_set_Vx_to_NN(Chip8* chip8, uint16_t opcode);
void chip8_op_add_NN_to_Vx(Chip8* chip8, uint16_t opcode);

// 0x8 Math Family

void chip8_op_set_Vx_to_Vy(Chip8* chip8, uint16_t opcode);
void chip8_op_set_Vx_to_Vx_or_Vy(Chip8* chip8, uint16_t opcode);
void chip8_op_set_Vx_to_Vx_and_Vy(Chip8* chip8, uint16_t opcode);
void chip8_op_set_Vx_to_Vx_xor_Vy(Chip8* chip8, uint16_t opcode);
void chip8_op_add_Vy_to_Vx(Chip8* chip8, uint16_t opcode);
void chip8_op_subtract_Vy_from_Vx(Chip8* chip8, uint16_t opcode);
void chip8_op_right_shift_Vx(Chip8* chip8, uint16_t opcode);
void chip8_op_set_Vx_to_Vy_minus_Vx(Chip8* chip8, uint16_t opcode);
void chip8_op_left_shift_Vx(Chip8* chip8, uint16_t opcode);

// Memory and Index

void chip8_op_skip_if_Vx_ne_Vy(Chip8* chip8, uint16_t opcode);
void chip8_op_set_I_to_NNN(Chip8* chip8, uint16_t opcode);
void chip8_op_jump_to_NNN_plus_V0(Chip8* chip8, uint16_t opcode);
void chip8_op_set_Vx_to_rand_and_NN(Chip8* chip8, uint16_t opcode);
void chip8_op_draw(Chip8* chip8, uint16_t opcode);

// Input and Timers

void chip8_op_skip_if_Vx_key_pressed(Chip8* chip8, uint16_t opcode);
void chip8_op_skip_if_Vx_key_not_pressed(Chip8* chip8, uint16_t opcode);
void chip8_op_get_delay_timer_in_Vx(Chip8* chip8, uint16_t opcode);
void chip8_op_get_keypress_in_Vx(Chip8* chip8, uint16_t opcode);
void chip8_op_set_delay_timer_to_Vx(Chip8* chip8, uint16_t opcode);
void chip8_op_set_sound_timer_to_Vx(Chip8* chip8, uint16_t opcode);
void chip8_op_add_Vx_to_I(Chip8* chip8, uint16_t opcode);
void chip8_op_set_I_to_sprite_address_in_Vx(Chip8* chip8, uint16_t opcode);
void chip8_op_store_Vx_as_BCD_at_address_in_I(Chip8* chip8, uint16_t opcode);
void chip8_op_reg_dump(Chip8* chip8, uint16_t opcode);
void chip8_op_reg_load(Chip8* chip8, uint16_t opcode);

#endif // CHIP8_OP_H
