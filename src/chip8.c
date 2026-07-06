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

void chip8_machine_fetch_and_decode(struct chip8_machine *const m) { 
    uint16_t instr = ((uint16_t)(m->memory[m->pc])) << 8 | (uint16_t) m->memory[m->pc + 1]; // Combine 8-bit memory[pc] and memory[pc+1] into single 2 byte instr

    uint8_t opcode = (uint8_t)((instr & 0xF000) >> 12);
    m->current_instruction.type = UNKNOWN; // Default instruction.
    m->current_instruction.x = (uint8_t)((instr & 0x0F00) >> 8); 
    m->current_instruction.y = (uint8_t)((instr & 0x00F0) >> 4);
    m->current_instruction.n = (uint8_t)((instr & 0x000F));
    m->current_instruction.nn = (uint8_t)((instr & 0x00FF));
    m->current_instruction.nnn = instr & 0x0FFF;

    switch (opcode) {  
        case 0x0: 
            switch (m->current_instruction.nnn) {
                case 0x0E0: 
                    m->current_instruction.type = CLS;
                    break;
                case 0x0EE:
                    m->current_instruction.type = RET;
                    break;
                default:
                    break; 
            }
            break;
        
        case 0x1:
            m->current_instruction.type = JP_A;
            break;
        
        case 0x2:
            m->current_instruction.type = CALL_A;
            break;
            
        case 0x3:
            m->current_instruction.type = SE_V_B;
            break;
        
        case 0x4:
            m->current_instruction.type = SNE_V_B;
            break;
        
        case 0x5:
            m->current_instruction.type = SE_V_V;
            break;
        
        case 0x6:
            m->current_instruction.type = LD_V_B;
            break;
        
        case 0x7:
            m->current_instruction.type = ADD_V_B;
            break;
        
        case 0x8:
            switch (m->current_instruction.n) {
                case 0x0:
                    m->current_instruction.type = LD_V_V; 
                    break;
                case 0x1:
                    m->current_instruction.type = OR_V_V;
                    break;
                case 0x2:
                    m->current_instruction.type = AND_V_V;
                    break;
                case 0x3:
                    m->current_instruction.type = XOR_V_V;
                    break;
                case 0x4:
                    m->current_instruction.type = ADD_V_V;
                    break;
                case 0x5:
                    m->current_instruction.type = SUB_V_V;
                    break;
                case 0x6:
                    m->current_instruction.type = SHR_V_V;
                    break;
                case 0x7:
                    m->current_instruction.type = SUBN_V_V;
                    break;
                case 0xE:
                    m->current_instruction.type = SHL_V_V;
                    break;
                default:
                    break;
            }
        
        case 0x9:
            switch (m->current_instruction.n) {
                case 0x0:
                    m->current_instruction.type = SNE_V_V;
                    break;
                default:
                    break; 
            }
        
        case 0xA:
            m->current_instruction.type = LD_I_A;
            break;
        
        case 0xB:
            m->current_instruction.type = JP_V_A;
            break;
        
        case 0xC:
            m->current_instruction.type = RND_V_B;
            break;
        
        case 0xD:
            m->current_instruction.type = DRW_V_V_N;
            break;
        
        case 0xE:
            switch (m->current_instruction.nn) {
                case 0x9E:
                    m->current_instruction.type = SKP_V;
                    break;
                case 0xA1:
                    m->current_instruction.type = SKNP_V;
                    break;
                case 0x07:
                    m->current_instruction.type = LD_V_DT;
                    break;
                case 0x0A:
                    m->current_instruction.type = LD_V_K;
                    break;
                case 0x15:
                    m->current_instruction.type = LD_DT_V;
                    break;
                case 0x18:
                    m->current_instruction.type = LD_ST_V;
                    break;
                case 0x1E:
                    m->current_instruction.type = ADD_I_V;
                    break;
                case 0x29:
                    m->current_instruction.type = LD_F_V;
                    break;
                case 0x33:
                    m->current_instruction.type = LD_B_V;
                    break;
                case 0x55:
                    m->current_instruction.type = LD_I_V;
                    break;
                case 0x65:
                    m->current_instruction.type = LD_V_I;
                    break;
                default:
                    break;
            }
        
        default:
            break;
    }

    m->pc += 2; // Incr. pc to start of next instruction.
}

void chip8_machine_execute(struct chip8_machine *const m) { 
    
}


