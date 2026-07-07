#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

// Chip 8 Machine Specs
#define CHIP8_MEMORY_SIZE 4096 
#define CHIP8_DISPLAY_WIDTH 64
#define CHIP8_DISPLAY_HEIGHT 32
#define CHIP8_STACK_SIZE 2
#define CHIP8_NUM_V_REGISTERS 16

// Keypad
#define KEYPAD_SIZE 16

// Special Addresses
#define CHIP8_PROGRAM_START 0x200
#define CHIP8_PROGRAM_FINAL 0xFFF
#define CHIP8_PROGRAM_MAX_SIZE ((CHIP8_PROGRAM_FINAL) - (CHIP8_PROGRAM_START) + 1)
#define CHIP8_RESERVED_START 0x000
#define CHIP8_RESERVED_FINAL 0x1FF
#define CHIP8_FONTSET_START 0x050
#define CHIP8_FONTSET_FINAL 0x09F
#define CHIP8_FONTSET_MAX_SIZE ((CHIP8_FONTSET_FINAL) - (CHIP8_FONTSET_START) + 1) 
#define FLAG_REGISTER_INDEX 0xF

// Return codes
#define EXEC_SUCCESS 0 // Successful instruction execution 
#define FETCH_DECODE_SUCCESS 0
#define ROM_LOAD_SUCCESS 0
#define ERR_UNKNOWN_INSTR -1 // Instruction is unknown/cannot be decoded.
#define ERR_STACK_OVERFLOW -2 // 
#define ERR_STACK_UNDERFLOW -3
#define ERR_INVALID_KEYCODE -4
#define ERR_INVALID_MEMORY_ADDR -5
#define ERR_FETCH_OUT_OF_BOUNDS -6
#define ERR_INVALID_ROM_FILEPATH -7
#define ERR_ROM_TOO_LARGE -8

// Display indexing
#define DISPLAY_INDEX(x, y) ((y) * (CHIP8_DISPLAY_WIDTH) + (x))

enum chip8_instruction_type {
    UNKNOWN, // Placeholder for unsuccessful decoding
    SYS,
    CLS, 
    RET,  
    JP_A, 
    CALL_A,
    SE_V_B,
    SNE_V_B,  
    SE_V_V,  
    LD_V_B, 
    ADD_V_B,
    LD_V_V, 
    OR_V_V, 
    AND_V_V,
    XOR_V_V,
    ADD_V_V,
    SUB_V_V,
    SHR_V_V,
    SUBN_V_V,
    SHL_V_V,
    SNE_V_V,
    LD_I_A,
    JP_V_A,
    RND_V_B,
    DRW_V_V_N,
    SKP_V,
    SKNP_V,
    LD_V_DT,
    LD_V_K,
    LD_DT_V,
    LD_ST_V,
    ADD_I_V,
    LD_F_V,
    LD_B_V,
    LD_I_V,
    LD_V_I,
};

struct chip8_instruction {
    enum chip8_instruction_type type; 
    uint8_t x; // The second nibble of the instr
    uint8_t y; // Third nibble
    uint8_t n; // Fourth nibble
    uint8_t nn; // The second byte (third and fourth nibbles)
    uint16_t nnn; // The second, third and fourth nibbles. (12-bit memory addr)
};

struct chip8_machine { 
    struct chip8_instruction current_instruction;
    uint16_t pc; // Array index into memory. 
    uint16_t stack[CHIP8_STACK_SIZE]; 
    uint16_t i; // I (index register) used to point at locations in memory
    uint8_t keypad[KEYPAD_SIZE];
    uint8_t memory[CHIP8_MEMORY_SIZE];
    uint8_t display[CHIP8_DISPLAY_HEIGHT * CHIP8_DISPLAY_WIDTH];
    uint8_t v[CHIP8_NUM_V_REGISTERS];
    uint8_t delay_timer; // 
    uint8_t sound_timer;
    uint8_t draw_flag;
    int8_t stack_i; // Index points to top item in the stack. -1 for empty stack.
};

struct chip8_machine *chip8_machine_create(void);
void chip8_machine_load_font(struct chip8_machine *const m, const uint8_t fontset[static CHIP8_FONTSET_MAX_SIZE]);
void chip8_machine_decrement_dt(struct chip8_machine *const m);
void chip8_machine_decrement_st(struct chip8_machine *const m);
int chip8_machine_load_rom_file(struct chip8_machine *const m, const char *const file_path);
int chip8_machine_fetch_and_decode(struct chip8_machine *const m);
int chip8_machine_execute(struct chip8_machine *const m);
void chip8_machine_reset_draw_flag(struct chip8_machine *const m);

#endif