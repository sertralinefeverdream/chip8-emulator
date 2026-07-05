#include "../include/chip8.h"

#include <stdint.h>
#include <stdlib.h>

struct chip8_machine *chip8_machine_create(void) {
    struct chip8_machine *m = malloc(sizeof(struct chip8_machine));

    if (!m) {
        return m;
    }

    memset(m, 0, sizeof(*m)); 
    m->pc = CHIP8_PROGRAM_START; // Initialise program counter to starting program addr
    return m;
}

void chip8_machine_load_font(struct chip8_machine *const m, uint8_t fontset[static CHIP8_FONTSET_MAX_SIZE]) { 
    for (size_t i = 0; i < CHIP8_FONTSET_MAX_SIZE; ++i) { 
        m->memory[CHIP8_FONTSET_START + i] = fontset[i];
    }
}

void chip8_machine_fetch(struct chip8_machine *const m) { 
    uint16_t instr = ((uint16_t)(m->memory[m->pc])) << 8 | (uint16_t) m->memory[m->pc + 1]; // Combine 8-bit memory[pc] and memory[pc+1] into single 2 byte instr

    m->current_instruction.opcode = (instr & 0xF000) >> 12;
    m->current_instruction.x = (uint8_t)((instr & 0x0F00) >> 8); 
    m->current_instruction.y = (uint8_t)((instr & 0x00F0) >> 4);
    m->current_instruction.n = (uint8_t)((instr & 0x000F));
    m->current_instruction.nn = (uint8_t)((instr & 0x00FF));
    m->current_instruction.nnn = instr & 0x0FFF;

    m->pc += 2;
}

void chip8_machine_decode_and_exec(struct chip8_machine *const m) {
    
}


