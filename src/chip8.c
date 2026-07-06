#include "../include/chip8.h"

#include <string.h>
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
    m->current_instruction.x = (uint8_t)((instr & 0x0F00) >> 8); 
    m->current_instruction.y = (uint8_t)((instr & 0x00F0) >> 4);
    m->current_instruction.n = (uint8_t)((instr & 0x000F));
    m->current_instruction.nn = (uint8_t)((instr & 0x00FF));
    m->current_instruction.nnn = instr & 0x0FFF;
    m->current_instruction.type = UNKNOWN; // Default instruction.

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
            break;
        
        case 0x9:
            switch (m->current_instruction.n) {
                case 0x0:
                    m->current_instruction.type = SNE_V_V;
                    break;
                default:
                    break; 
            }
            break;
        
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

static int exec_instr_display(struct chip8_machine *const m) {
    const uint8_t x = m->v[m->current_instruction.x] % CHIP8_DISPLAY_WIDTH;
    const uint8_t y = m->v[m->current_instruction.y] % CHIP8_DISPLAY_HEIGHT; // Modulo to wrap
    
    m->v[FLAG_REGISTER_INDEX] = 0;
    for (size_t j = 0; j < m->current_instruction.n; ++j) { 
        if (y + j > CHIP8_DISPLAY_HEIGHT - 1) {
            continue;
        }

        uint8_t sprite = m->v[m->i + j];
        for (size_t k = 0; k < 8; ++k) {            
            if (x + k > CHIP8_DISPLAY_WIDTH - 1) {
                continue;
            }
            
            uint8_t display_pixel = m->display[DISPLAY_INDEX(x+k, y+k)];
            uint8_t sprite_pixel = (sprite >> (7-k)) & 0x1; 
            if (display_pixel && sprite_pixel) {
                m->v[FLAG_REGISTER_INDEX] = 1;
            }
        
            m->display[DISPLAY_INDEX(x+k, y+k)] = display_pixel ^ sprite_pixel;
        }
    }
    
    return EXEC_SUCCESS;
} 



int chip8_machine_execute(struct chip8_machine *const m) {  
    switch(m->current_instruction.type) {
        case SYS:
            break; // SYS essentially should do nothing.
        
        case CLS:
            for (size_t i = 0; i < CHIP8_DISPLAY_HEIGHT * CHIP8_DISPLAY_WIDTH; ++i) {
                m->display[i] = 0;
            } 
            break; 
        
        case RET:
            if (m->stack_i + 1 <= -1) {
                return ERR_STACK_UNDERFLOW;
            }
            
            m->pc = m->stack[m->stack_i];
            m->stack_i--;
            break;
            
        case JP_A:
            m->pc = m->current_instruction.nnn;
            break;

        case CALL_A:
            if (m->stack_i + 1 >= CHIP8_STACK_SIZE) {
                return ERR_STACK_OVERFLOW;
            }
    
            m->stack_i++;
            m->stack[m->stack_i] = m->pc; 
            m->pc = m->current_instruction.nnn; 
            break;
        
        case SE_V_B:
            if (m->v[m->current_instruction.x] == m->current_instruction.nn) { 
                m->pc += 2;
            }
            break;
        
        case SNE_V_B:
            if (m->v[m->current_instruction.x] != m->current_instruction.nn) {
                m->pc += 2;
            }
            break;
        
        case SE_V_V:
            if (m->v[m->current_instruction.x] == m->v[m->current_instruction.y]) {
                m->pc += 2;
            }
            break;
        
        case LD_V_B:
            m->v[m->current_instruction.x] = m->current_instruction.nn;
            break;
        
        case ADD_V_B:
            m->v[m->current_instruction.x] += m->current_instruction.nn;
            break;
        
        case LD_V_V:
            m->v[m->current_instruction.x] = m->v[m->current_instruction.y];
            break;
        
        case OR_V_V:
            m->v[m->current_instruction.x] |= m->v[m->current_instruction.y];
            break;
        
        case AND_V_V:
            m->v[m->current_instruction.x] &= m->v[m->current_instruction.y];
            break;
        
        case XOR_V_V:
            m->v[m->current_instruction.x] ^= m->v[m->current_instruction.y];
            break;
        
        case ADD_V_V:
            if (m->v[m->current_instruction.y] > UINT8_MAX - m->v[m->current_instruction.x]) {
                m->v[FLAG_REGISTER_INDEX] = 1; // Signifies that the operation has overflowed.
            }
            
            m->v[m->current_instruction.x] += m->v[m->current_instruction.y];
            break;
        
        case SUB_V_V:
            if (m->v[m->current_instruction.x] >= m->v[m->current_instruction.y]) {
                m->v[FLAG_REGISTER_INDEX] = 1;
            } else {
                m->v[FLAG_REGISTER_INDEX] = 0;
            }
            
            m->v[m->current_instruction.x] -= m->v[m->current_instruction.y];
            break;
        
        case SUBN_V_V:
            if (m->[m->current_instruction.y] >= m->v[m->current_instruction.x]) {
                m->v[FLAG_REGISTER_INDEX] = 1;
            } else {
                m->v[FLAG_REGISTER_INDEX] = 0;
            }
            
            m->v[m->current_instruction.x] = m->v[m->current_instruction.y] - m->v[m->current_instruction.x];
            break;
        
        case SHR_V_V:
            // Ambiguous instruction. First step is not in chip-48 or later
            m->v[m->current_instruction.x] = m->v[m->current_instruction.y];
            m->v[FLAG_REGISTER_INDEX] = m->v[m->current_instruction.x] & 0x1; // Getting LSB 
            m->v[m->current_instruction.x] >>= 1;
            break;
        
        case SHL_V_V:
            // Ambiguous instruction. Ditto
            m->v[m->current_instruction.x] = m->v[m->current_instruction.y];
            m->v[FLAG_REGISTER_INDEX] = m->v[m->current_instruction.x] >> 7; // Getting MSB
            m->v[m->current_instruction.x] <<= 1;
            break;
        
        case SNE_V_V:
            if (m->v[m->current_instruction.x] != m->v[m->current_instruction.y]) {
                m->pc += 2; 
            }
            break;
        
        case LD_I_A:
            m->i = m->current_instruction.nnn;
            break;
        
        case JP_V_A:
            // Implem. with original COSMAC VIP behaviour as described on tobiasvl source
            m->pc = m->current_instruction.nnn + m->v[0];
            break;
        
        case RND_V_B:
            m->v[m->current_instruction.x] = rand() & m->current_instruction.nn;
            break;
        
        case DRW_V_V_N:
            exec_instr_display(m);
            break;

        default:
            return ERR_UNKNOWN_INSTR; 
    }
    
    return EXEC_SUCCESS;
}


