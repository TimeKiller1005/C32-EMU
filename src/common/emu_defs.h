/*
 *--------------------------------------------------------------------------
 * Copyright (C) 2013, Joe Mruz
 * Contact email: mruz.joe@gmail.com
 *
 * This file is part of C32-EMU.
 *
 * C32-EMU is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * C32-EMU is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with C32-EMU.  If not, see <http://www.gnu.org/licenses/>.
 *--------------------------------------------------------------------------
 */
/*
 *--------------------------------------------------
 * File: emu_defs.h
 * Description:
 *		Common Definitions
 * author: joe mruz
 *--------------------------------------------------
 */

#ifndef _EMU_DEFS_H_
#define _EMU_DEFS_H_

#include <stdint.h>


/* instruction set */
enum is
{
    NOP = 0,
    SET,
    MOV,
    LOAD,
    STORE,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    NEG,
    AND,
    BOR,
    XOR,
    BSL,
    BSR,
    CMP,
    IN,
    OUT,
    M32,
    M16,
    M8,
    HLD,
    HLT,
    PUSH,
    POP,
    CALL,
    JMP,
    JR,
    JZ,
    JNZ,
    JS,
    JNS,
    RET,
    OPTIMIZED_CPY,
    CR,
    CZ,
    CNZ,
    CS,
    CNS,
    DSET,
    DCALL,
    DJMP,
    DJR,
    DJZ,
    DJNZ,
    DJS,
    DJNS,
    DRET,
    DCR,
    DCZ,
    DCNZ,
    DCS,
    DCNS
};

/* 32 bit instruction */
/*
 *	8 bits 			4 bits	 	4 bits		16 bits
 * [OPCODE			| dst		| src		| constant]
 */
struct instr_s
{
    uint8_t     opcode;
    uint8_t     dst_src;	/* dst in high 4 bits, src in lower 4 bits */
    uint16_t	constant;
};

/* Registers */
#define REGISTERS (16)
enum reg
{
    REGISTER_A = 0,
    REGISTER_B,
    REGISTER_C,
    REGISTER_X,
    REGISTER_Y,
    REGISTER_Z,
    REGISTER_COMPARE,   /*CFLAGS*/
    REGISTER_PORT,
    REGISTER_TIMERQ,    /* time quantum (# of cycles) */
    REGISTER_TIMERA,    /* address for time jmp */
    REGISTER_ACC,
    REGISTER_OFFSET,
    REGISTER_STACKBASE,
    REGISTER_STACKPOINTER,
    REGISTER_PC,
    REGISTER_ZERO
};

/* CPU state */
enum state
{
    STATE_RUNNING,
    STATE_HOLD,
    STATE_HALT
};

/* Ports */
#define MAX_PORTS (4)
enum ports
{
	PORT_DISPLAY = 0,
	PORT_KEYBOARD,
	PORT_DISK,
    PORT_ROM
};

#endif	/* _EMU_DEFS_H_ */
