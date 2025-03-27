#pragma once

#include <common.h>
#include <instructions.h>

typedef struct {
    u8 a;
    u8 f;
    u8 b;
    u8 c;
    u8 d;
    u8 e;
    u8 h;
    u8 l;
    u16 pc;
    u16 sp;
} cpu_registers;

typedef struct {
    cpu_registers regs;

    //current fetch...
    u16 fetched_data;
    u16 mem_dest;
    bool dest_is_mem;
    u8 cur_opcode;
    instruction *cur_inst;

    bool halted;
    bool stepping;

    bool int_master_enabled;
    
} cpu_context;

void cpu_init();
bool cpu_step();
//cpu_context * means it takes a pointer to the cpu_context as an parameter
typedef void (*IN_PROC)(cpu_context *);

//get processor by instruction type, so we can have a processor function for each instruction
IN_PROC inst_get_processor(in_type type);

/*
Flag register (F) bits:

7 	6 	5 	4 	3 	2 	1 	0
Z 	N 	H 	C 	0 	0 	0 	0

Z - Zero Flag
N - Subtract Flag
H - Half Carry Flag
C - Carry Flag
0 - Not used, always zero
*/
#define CPU_FLAG_Z BIT(ctx->regs.f, 7)
#define CPU_FLAG_C BIT(ctx->regs.f, 4)

u16 cpu_read_reg(reg_type rt);

u16 cpu_set_reg(reg_type rt, u16 val);

