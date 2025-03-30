#include <instructions.h>
#include <cpu.h>

// https://pastraiser.com/cpu/gameboy/gameboy_opcodes.html
instruction instructions[0x100] = {
    [0x00] = {IN_NOP, AM_IMP},
    [0x01] = {IN_LD, AM_R_D16, RT_BC},
    [0x02] = {IN_LD, AM_MR_R, RT_BC, RT_A},
    [0x03] = {IN_INC, AM_R, RT_BC},
    [0x04] = {IN_INC, AM_R, RT_B},
    [0x05] = {IN_DEC, AM_R, RT_B},
    /*
        0x06 = opcode
        addressing mode R_D8 means load 8 bytes from memory, then load into a register (B).
        d = data, referring to an immediate value. in this case 8 bits.
    */
    [0x06] = {IN_LD, AM_R_D8, RT_B},
    [0x07] = {IN_RLCA, RT_A}, //Rotate left carry A
    /*
        a16 = 16bit memory address.
        load the contents of the SP register to the 16bit memory address.
    */
    [0x08] = {IN_LD, AM_R_A16, RT_NONE, RT_SP},
    [0x09] = {IN_ADD, AM_R_R, RT_HL, RT_BC},
    [0x0A] = {IN_LD, AM_R_R, RT_A, RT_BC},
    [0x0B] = {IN_DEC, AM_R, RT_BC},
    [0x0C] = {IN_INC, AM_R, RT_C},
    [0x0D] = {IN_DEC, AM_R, RT_C},
    [0x0E] = {IN_LD, AM_R_D8, RT_C},
    [0x0F] = {IN_RRCA, RT_A}, //Rotate right carry A
    [0x10] = {IN_STOP, AM_IMP},
    [0x11] = {IN_LD, AM_R_D16, RT_DE},
    [0x12] = {IN_LD, AM_MR_R, RT_DE, RT_A},
    [0x13] = {IN_INC, AM_R, RT_DE},
    [0x14] = {IN_INC, AM_R, RT_D},
    [0x15] = {IN_DEC, AM_R, RT_D},
    [0x16] = {IN_LD, AM_R_D8, RT_D},
    [0x17] = {IN_RLA, RT_A}, //Rotate left A

    [0x19] = {IN_ADD, AM_R_R, RT_HL, RT_BC},
    [0x20] = {IN_JR, AM_R, RT_F},
    [0x21] = {IN_LD, AM_R_D16, RT_HL},
    [0x22] = {IN_LD, AM_HLI_R, RT_HL, RT_A},//HL+
    [0x23] = {IN_INC, AM_R, RT_HL},
    [0x24] = {IN_INC, AM_R, RT_H},
    [0x25] = {IN_DEC, AM_R, RT_H},
    [0x26] = {IN_LD, AM_R_D8, RT_H},




    // load 16bits of immediate data into SP register
    [0x31] = {IN_LD, AM_R_D16, RT_SP},
    [0x32] = {IN_LD, AM_R_HLD, RT_HL, RT_A},//HL-
    [0x33] = {IN_INC, AM_R, RT_SP},
    [0x34] = {IN_INC, AM_R, RT_HL},
    [0x35] = {IN_DEC, AM_R, RT_HL},
    [0x36] = {IN_LD, AM_R_D8, RT_HL},

    [0x90] = {IN_SUB, AM_R_R, RT_A, RT_B},
    [0x91] = {IN_SUB, AM_R_R, RT_A, RT_C},
    [0x92] = {IN_SUB, AM_R_R, RT_A, RT_D},
    [0x93] = {IN_SUB, AM_R_R, RT_A, RT_E},
    [0x94] = {IN_SUB, AM_R_R, RT_A, RT_H},
    [0x95] = {IN_SUB, AM_R_R, RT_A, RT_L},
    [0x96] = {IN_SUB, AM_R_R, RT_A, RT_HL},
    [0x97] = {IN_SUB, AM_R_R, RT_A, RT_A}, //set A to zero. quickest way to clear A and set Z
    [0xD6] = {IN_SUB, AM_R_D8, RT_A},
    [0x76] = {IN_HALT},


    [0xAF] = {IN_XOR, AM_R, RT_A},
    /*
        Example:
        200: C3 50 01  ; JP $0150
        C3 - opcode
        50 - low byte of d16
        01 - high byte of d16
        jumps to address 0x0150
    */
    [0xC3] = {IN_JP, AM_D16},

    [0xF3] = {IN_DI}
};

instruction *instruction_by_opcode(u8 opcode) {
    return &instructions[opcode];
}

char *inst_lookup[] = {
    "<NONE>",
    "NOP",
    "LD",
    "INC",
    "DEC",
    "RLCA",
    "ADD",
    "RRCA",
    "STOP",
    "RLA",
    "JR",
    "RRA",
    "DAA",
    "CPL",
    "SCF",
    "CCF",
    "HALT",
    "ADC",
    "SUB",
    "SBC",
    "AND",
    "XOR",
    "OR",
    "CP",
    "POP",
    "JP",
    "PUSH",
    "RET",
    "CB",
    "CALL",
    "RETI",
    "LDH",
    "JPHL",
    "DI",
    "EI",
    "RST",
    "IN_ERR",
    "IN_RLC", 
    "IN_RRC",
    "IN_RL", 
    "IN_RR",
    "IN_SLA", 
    "IN_SRA",
    "IN_SWAP", 
    "IN_SRL",
    "IN_BIT", 
    "IN_RES", 
    "IN_SET"
};

char *inst_name(in_type t) {
    return inst_lookup[t];
}
