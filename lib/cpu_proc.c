#include <cpu.h>
#include <emu.h>
#include <log.h>
#include <bus.h>

static void proc_none(cpu_context *ctx) {
    // log_write("INVALID INSTRUCTION!\n");
    // exit(-7);
}

static void proc_nop(cpu_context *ctx) {

}

static void proc_di(cpu_context *ctx) {
    ctx->int_master_enabled = false;
}

static void proc_ld(cpu_context *ctx) {
    //if the instruction is to load from a register to a memory address
    if(ctx->dest_is_mem){
        //if 16 bit register
        if(ctx->cur_inst->reg_2 >= RT_AF){
            //writing to an extra address, so its an extra cycle
            emu_cycles(1);
            bus_write16(ctx->mem_dest, ctx->fetched_data);
        } else {
            bus_write(ctx->mem_dest, ctx->fetched_data);
        }
    }
    else
    {
        //LD HL,SP+r8
        if(ctx->cur_opcode == 0xF8){
            //add r8(signed 8 bits) to PC and set to HL 
            cpu_set_reg(ctx->cur_inst->reg_1, cpu_read_reg(ctx->cur_inst->reg_2) + ctx->fetched_data);

            // cpu_set_flags(
            //     ctx,
            //     0,
            //     0,
            //     (ctx->cur_inst->reg_2 & 0x0FFF) + (ctx->fetched_data & 0x0FFF) > 0x0FFF, 
            //     ctx->regs.a > 0xFFFF
            // );
        }
    }
    
    emu_cycles(1);
    
    //if vice versa
    cpu_set_reg(ctx->cur_inst->reg_1, ctx->fetched_data);
}

static void proc_inc(cpu_context *ctx) {
    cpu_set_reg(ctx->cur_inst->reg_1, cpu_read_reg(ctx->cur_inst->reg_1) + 1);
}

static void proc_dec(cpu_context *ctx) {
    cpu_set_reg(ctx->cur_inst->reg_1, cpu_read_reg(ctx->cur_inst->reg_1) - 1);
}

void cpu_set_flags(cpu_context *ctx, char z, char n, char h, char c) {
    if (z != -1) {
        BIT_SET(ctx->regs.f, 7, z);
    }

    if (n != -1) {
        BIT_SET(ctx->regs.f, 6, n);
    }

    if (h != -1) {
        BIT_SET(ctx->regs.f, 5, h);
    }

    if (c != -1) {
        BIT_SET(ctx->regs.f, 4, c);
    }
}

static void proc_xor(cpu_context *ctx) {
    ctx->regs.a ^= ctx->fetched_data & 0xFF;
    cpu_set_flags(ctx, ctx->regs.a == 0, 0, 0, 0);
}

static void proc_add(cpu_context *ctx){
    //ADD A,d8
    //Z 0 H C
    if(ctx->cur_opcode == 0xC6){
        //fethed data is d8 from bus
        cpu_set_reg(cpu_read_reg(ctx->cur_inst->reg_1), ctx->fetched_data);

        cpu_set_flags(
            ctx, 
            ctx->regs.a == 0, 
            0, 
            //If the sum of these lower 4 bits exceeds 0xF (binary 1111), then there was a carry from bit 3 to bit 4, so H = 1.
            (ctx->regs.a & 0x0F) + (ctx->fetched_data & 0x0F) > 0x0F, 
            ctx->regs.a > 0xFF
        );

        return;
    }

    // ADD SP,r8
    // 0 0 H C
    // if(ctx->cur_opcode == 0xE8){
    //     cpu_set_reg(cpu_read_reg(ctx->cur_inst->reg_1), ctx->fetched_data);

    //     cpu_set_flags(
    //         ctx, 
    //         0, 
    //         0, 
    //         //If the sum of these lower 4 bits exceeds 0xF (binary 1111), then there was a carry from bit 3 to bit 4, so H = 1.
    //         (ctx->regs.a & 0x0F) + (ctx->fetched_data & 0x0F) > 0x0F, 
    //         ctx->regs.a > 0xFF
    //     );

    //     return;
    // }
    
    u16 reg1 = cpu_read_reg(ctx->cur_inst->reg_1);
    //Address mode R_R in cpu_fetch.c set fetched data to ctx->cur_inst->reg_2 already
    cpu_set_reg(ctx->cur_inst->reg_1, reg1 + ctx->fetched_data);

    //16 bit 
    if(ctx->cur_inst->reg_1 >= RT_AF){
        cpu_set_flags(
            ctx, 
            reg1 == 0, 
            0, 
            (reg1 & 0x0FFF) + (ctx->fetched_data & 0x0FFF) > 0x0FFF, 
            reg1 > 0xFFFF
        );
    } else {
        cpu_set_flags(
            ctx, 
            reg1 == 0, 
            0, 
            (reg1 & 0x0F) + (ctx->fetched_data & 0x0F) > 0x0F, 
            reg1 > 0xFF
        );
    }
}

static void proc_sub(cpu_context *ctx) {
    //All sub instructions implicitly substract from A.
    bool carryBit = ctx->regs.a < ctx->fetched_data;

    ctx->regs.a - ctx->fetched_data;

    cpu_set_flags(
        ctx, 
        ctx->regs.a == 0, 
        1, 
        (ctx->regs.a & 0x0F) < (ctx->fetched_data & 0x0F), 
        carryBit
    );
}


static bool check_cond(cpu_context *ctx) {
    bool z = CPU_FLAG_Z;
    bool c = CPU_FLAG_C;

    switch(ctx->cur_inst->cond) {
        case CT_NONE: return true;
        case CT_C: return c;
        case CT_NC: return !c;
        case CT_Z: return z;
        case CT_NZ: return !z;
    }

    return false;
}

static void proc_jp(cpu_context *ctx) {
    if (check_cond(ctx)) {
        ctx->regs.pc = ctx->fetched_data;
        emu_cycles(1);
    }
}

static IN_PROC processors[] = {
    [IN_NONE] = proc_none,
    [IN_NOP] = proc_nop,
    [IN_LD] = proc_ld,
    [IN_JP] = proc_jp,
    [IN_DI] = proc_di,
    [IN_XOR] = proc_xor,
    [IN_INC] = proc_inc,
    [IN_DEC] = proc_dec,
    [IN_ADD] = proc_add,
    [IN_SUB] = proc_sub
};

IN_PROC inst_get_processor(in_type type) {
    return processors[type];
}