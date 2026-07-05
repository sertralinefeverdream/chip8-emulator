#ifndef CHIP8_H
#define CHIP8_H
#include <stdbool.h>
#include <stdint.h>

// Chip 8 Machine Specs
#define CHIP8_MEMORY_SIZE 4096 
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_STACK_SIZE 2
#define CHIP8_NUM_V_REGISTERS 16

// Special Addresses
#define CHIP8_PROGRAM_START 0x200
#define CHIP8_PROGRAM_FINAL 0xFFF
#define CHIP8_PROGRAM_MAX_SIZE ((CHIP8_PROGRAM_END) - (CHIP8_PROGRAM_START) + 1)
#define CHIP8_RESERVED_START 0x000
#define CHIP8_RESERVED_FINAL 0x1FF
#define CHIP8_FONTSET_START 0x050
#define CHIP8_FONTSET_FINAL 0x09F
#define CHIP8_FONTSET_MAX_SIZE ((CHIP8_FONTSET_FINAL) - (CHIP8_FONTSET_START) + 1) 

struct chip8_instruction {};

struct chip8_machine { 
    struct chip8_instruction current_instruction;
    uint8_t memory[CHIP8_MEMORY_SIZE];
    uint8_t display[CHIP8_DISPLAY_HEIGHT * CHIP8_DISPLAY_WIDTH];
    uint8_t v[CHIP8_NUM_V_REGISTERS];
    uint16_t pc; // Array index into memory. 
    uint16_t stack[CHIP8_STACK_SIZE]; 
    uint16_t i; // I (index register) used to point at locations in memory
    uint8_t stack_top; // Register used to point to top of stack
    uint8_t delay_timer; // 
    uint8_t sound_timer;
};

struct chip8_machine *chip8_machine_create(void);

void chip8_machine_load_font(struct chip8_machine *const m, uint8_t fontset[static CHIP8_FONTSET_MAX_SIZE]);
#endif