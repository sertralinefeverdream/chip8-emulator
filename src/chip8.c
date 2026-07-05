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

