#include <cpu.h>
#include <bus.h>
#include <emu.h>

extern cpu_context ctx;

/*
    Read 16 bits from the program counter (PC) register.
    Takes low and high 8 bits in PC and joins them together using bit shift and OR.

    Example:
    hi = 0000 0000 0001 0010  (0x0012)
    lo = 0000 0000 0011 0100  (0x0034)
    
    0001 0010 0000 0000   (hi << 8) Shift the bits up 8 bits, leaving the low 8 bits as 0's.
    0000 0000 0011 0100   (lo |) Low bits in hi are 0's so they get replaced by the 1's in lo.
    0001 0010 0011 0100   Result
*/
static u16 read_PC() {
    u16 lo = bus_read(ctx.regs.pc);
    emu_cycles(1);

    u16 hi = bus_read(ctx.regs.pc + 1);
    emu_cycles(1);

    return lo | (hi << 8);
}

static void fetch_data() {
    ctx.mem_dest = 0;
    ctx.dest_is_mem = false;
    
    if (ctx.cur_inst == NULL) {
        return;
    }

    switch(ctx.cur_inst->mode) {
        case AM_IMP: return;

        case AM_R:        
            //op on a single register, so just fetch the first register
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_1);
            return;

        case AM_R_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            return;

        case AM_R_D8:
            //read 8 bits from the program counter register
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case AM_R_D16:
        case AM_D16: {
            ctx.fetched_data = read_PC();
            return;
        } 

        case AM_MR_R:
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;

            /* 
                The full 16-bit absolute address is obtained by setting the most significant byte to 0xFF and
                the least significant byte to the value of C, so the possible range is 0xFF00-0xFFFF.
            */
            if(ctx.cur_inst->reg_1 == RT_C){
                ctx.mem_dest |= 0xFF00;
            }

            return;
        
        case AM_R_MR:{
            u16 addr = cpu_read_reg(ctx.cur_inst->reg_2);

            if(ctx.cur_inst->reg_1 == RT_C){
                addr |= 0xFF00;
            }

            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);
            return;
        }
        
        // I for Incr the HL register
        case AM_R_HLI:
            ctx.fetched_data = bus_read(ctx.cur_inst->reg_2);
            emu_cycles(1);
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            return;

        case AM_R_HLD:
            ctx.fetched_data = bus_read(ctx.cur_inst->reg_2);
            emu_cycles(1);
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            return;

        case AM_HLI_R:
            ctx.fetched_data = bus_read(ctx.cur_inst->reg_2);
            ctx.mem_dest = bus_read(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
            return;
        
        case AM_HLD_R:
            ctx.fetched_data = bus_read(ctx.cur_inst->reg_2);
            ctx.mem_dest = bus_read(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
            return;

        case AM_R_A8:
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return;
        
        case AM_A8_R:
            ctx.mem_dest = bus_read(ctx.regs.pc) | 0xFF00;
            ctx.dest_is_mem = true;
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case AM_HL_SPR:
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case AM_D8:
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            return;

        case AM_A16_R:
        case AM_D16_R:{
            ctx.mem_dest = read_PC();
            ctx.dest_is_mem = true;
            ctx.regs.pc += 2;  
            ctx.fetched_data = cpu_read_reg(ctx.cur_inst->reg_2);
            return;
        } 

        case AM_MR_D8:
            ctx.fetched_data = bus_read(ctx.regs.pc);
            emu_cycles(1);
            ctx.regs.pc++;
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            return;
            
        case AM_MR:
            ctx.mem_dest = cpu_read_reg(ctx.cur_inst->reg_1);
            ctx.dest_is_mem = true;
            ctx.fetched_data = bus_read(cpu_read_reg(ctx.cur_inst->reg_1));
            emu_cycles(1);
            return;

        case AM_R_A16: {
            u16 addr = read_PC();
            ctx.regs.pc += 2;  
            ctx.fetched_data = bus_read(addr);
            emu_cycles(1);
            return;
        } 

        default:
            log_write("Unknown Addressing Mode! %d (%02X)\n", ctx.cur_inst->mode, ctx.cur_opcode);
            exit(-7);
            return;
    }
}
