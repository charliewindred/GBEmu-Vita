#include <cpu.h>
#include <bus.h>
#include <emu.h>
#include <log.h>

cpu_context ctx = {0};

/*
    After the BIOS finishes execution, the registers are:
    Register	Initial Value
    A	        0x01
    F	        0xB0
    BC	        0x0013
    DE	        0x00D8
    HL	        0x014D
    SP	        0xFFFE
    PC	        0x0100
*/
void cpu_init() {
    ctx.regs.pc = 0x100;
    ctx.regs.a = 0x01;
}

static void fetch_instruction() {
    ctx.cur_opcode = bus_read(ctx.regs.pc++);
    ctx.cur_inst = instruction_by_opcode(ctx.cur_opcode);
}

void fetch_data();

static void execute() {
    IN_PROC proc = inst_get_processor(ctx.cur_inst->type);

    if (!proc) {
        NO_IMPL
    }

    proc(&ctx);
}

bool cpu_step() {
    
    if (!ctx.halted) {
        u16 pc = ctx.regs.pc;

        fetch_instruction();
        fetch_data();

        log_write("%04X: %-7s (%02X %02X %02X) A: %02X B: %02X C: %02X\n", 
            pc, inst_name(ctx.cur_inst->type), ctx.cur_opcode,
            bus_read(pc + 1), bus_read(pc + 2), ctx.regs.a, ctx.regs.b, ctx.regs.c);

        if (ctx.cur_inst == NULL) {
            log_write("Unknown Instruction! %02X\n", ctx.cur_opcode);
            exit(-7);
        }

        execute();
    }

    return true;
}