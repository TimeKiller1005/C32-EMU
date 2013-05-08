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
 * File: emu_cpu.c
 * Description: Interpret instructions
 *--------------------------------------------------
 */

#include <string.h>
#include "emu.h"
#include "emu_utils.h"

/*
 *--------------------------------------------------
 * Function: cpu_init
 * Description:	
 *		initialize cpu object fields
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
cpu_init(cpu_t *cpu)
{	
	memset (cpu->registers, 0, sizeof (cpu->registers));
	memset (cpu->ports, 0, sizeof (cpu->ports));
	
	cpu->adr_mode 							= 32;
	cpu->state           					= STATE_HALT;
	cpu->registers [REGISTER_STACKPOINTER] 	= 0xFFFFF - 8;
	cpu->registers [REGISTER_STACKBASE]		= 0xFFFFF - 4;
	cpu->registers [REGISTER_OFFSET] 		= 0;
	cpu->registers [REGISTER_PC] 			= 0;
    cpu->cycle                              = 0;
}

/*
 *--------------------------------------------------
 * Function: cpu_process
 * Description:	
 *		unpack, interpret instructions
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
cpu_process(cpu_t *cpu, ram_t *ram)
{
	uint8_t     opcode, dst, src, dst_src;
	uint16_t    constant;
	uint32_t    ins, long_const;
	
	/*auto-halt if the instruction pointer is at or over the bounds of the memory*/
	if (((unsigned int)cpu->registers [REGISTER_PC]) >= MEMORY_SIZE - 4)
	{
		cpu->state = STATE_HALT;
        printf ("Halt. Program counter out of bounds.\n");
		return;
	}
	
	utils_load (ram, &ins, cpu->registers [REGISTER_PC], 32);
	
	/* unpack */
	opcode		= (ins >> 24) & 0xff;
	dst_src		= (ins >> 16) & 0xff;
	constant	= ins & 0xffff;
	dst			= (dst_src >> 4) & 0x0f;
	src 		= (dst_src & 0x0f);
	long_const	= ((src << 16) | constant) & 0x000FFFFF;

	/* interpret */
	switch (opcode)
	{
		case NOP: 
			break;
		case SET: 
			cpu->registers [dst] = long_const; 
			break;
		case MOV: 
			cpu->registers [dst] = cpu->registers [src]; 
			break;
		case LOAD: 
			utils_load (ram, 
                        &cpu->registers [dst], 
                        cpu->registers [src], 
                        cpu->adr_mode); 
            break;
		case STORE:	
			utils_store (ram, 
                         cpu->registers [src], 
                         cpu->registers [dst], 
                         cpu->adr_mode); 
            break;
		case ADD: cpu->registers [dst] = cpu->registers [dst] + 
										 cpu->registers [src]; 
			break;
		case SUB: cpu->registers [dst] = cpu->registers [dst] - 
										 cpu->registers [src]; 
			break;
		case MUL: cpu->registers [dst] = cpu->registers [dst] * 
										 cpu->registers [src]; 
			break;
		case DIV: cpu->registers [dst] = cpu->registers [dst] / 
										 cpu->registers [src]; 
			break;
		case MOD: cpu->registers [dst] = cpu->registers [dst] % 
										 cpu->registers [src]; 
			break;
		case NEG: cpu->registers [dst] = !cpu->registers [dst]; 
			break;
		case AND: cpu->registers [dst] = cpu->registers [dst] & 
										 cpu->registers [src]; 
			break;
		case BOR: cpu->registers [dst] = cpu->registers [dst] | 
										 cpu->registers [src]; 
			break;
		case XOR: cpu->registers [dst] = cpu->registers [dst] ^ 
										 cpu->registers [src]; 
			break;
		case BSL: cpu->registers [dst] = cpu->registers [dst] << 
										 cpu->registers [src]; 
			break;
		case BSR: cpu->registers [dst] = cpu->registers [dst] >> 
										 cpu->registers [src]; 
			break;
		case CMP: utils_compare (&cpu->registers [REGISTER_CFLAGS], 
                                  cpu->registers [dst], 
                                  cpu->registers [src]); 
            break;
		case M32: 
			cpu->adr_mode = 32; 
			break;
		case M16: 
			cpu->adr_mode = 16; 	
			break;
		case M8:  
			cpu->adr_mode = 8;	
			break;
        case IN: 
			cpu->registers [dst] = cpu->ports [cpu->registers [REGISTER_PORT]]; 
			break;
        case OUT: 
            if (cpu->registers [REGISTER_PORT] >= MAX_PORTS) 
            {
                printf ("Warning: invalid port %d in OUT instruction\n",
						cpu->registers [REGISTER_PORT]);
                /*DO NOTHING*/
            }
            else 
                cpu->ports [cpu->registers [REGISTER_PORT]] = cpu->registers [dst];
            break;
        case HLD: 
			cpu->state = STATE_HOLD; 
			break;
        case HLT: 
			cpu->state = STATE_HALT;
			break;
		case PUSH:
            cpu->registers [REGISTER_STACKPOINTER] -= 4;
            utils_store (ram, 
					     cpu->registers [dst], 
						 cpu->registers [REGISTER_STACKPOINTER], 
						 32);
            break;
		case POP:
            utils_load (ram, 
					    &cpu->registers [dst], 
						cpu->registers [REGISTER_STACKPOINTER], 
						32);
            cpu->registers [REGISTER_STACKPOINTER] += 4;
            break;
		case CALL: 
            cpu->registers [REGISTER_STACKPOINTER] -= 4;
            utils_store (ram, 
						 cpu->registers [REGISTER_PC], 
						 cpu->registers[REGISTER_STACKPOINTER], 
						 32);
            cpu->registers [REGISTER_PC] = long_const; 
            break;
		case JMP: 
			cpu->registers [REGISTER_PC] = long_const; 
			break;
        case JR: 
			cpu->registers [REGISTER_PC] = cpu->registers [dst]; 
			break;
        case JZ: 
            if (cpu->registers [REGISTER_CFLAGS] & 0x00000001)
                cpu->registers [REGISTER_PC] = long_const;
            break;
        case JNZ:
            if (! (cpu->registers [REGISTER_CFLAGS] & 0x00000001))
                cpu->registers [REGISTER_PC] = long_const;
            break;
        case JS:
            if (cpu->registers [REGISTER_CFLAGS] & 0x00000002)
                cpu->registers [REGISTER_PC] = long_const;
            break;
        case JNS:
            if (! (cpu->registers [REGISTER_CFLAGS] & 0x00000002))
                cpu->registers [REGISTER_PC] = long_const;
            break;
		case RET:
            utils_load (ram, 
            			&cpu->registers [REGISTER_PC], 
            			cpu->registers [REGISTER_STACKPOINTER], 
            			32);
            cpu->registers [REGISTER_STACKPOINTER] += 4;
            break;
		case OPCP: 
            utils_cpy_chunk (ram, 
                             cpu->registers [dst], 
                             cpu->registers [src], 
                             1024*150); 
            break;
        case CR: 
        	cpu->registers [REGISTER_STACKPOINTER] -= 4;
            utils_store (ram, 
            			 cpu->registers [REGISTER_PC], 
            			 cpu->registers[REGISTER_STACKPOINTER], 
            			 32);
            cpu->registers [REGISTER_PC] = cpu->registers [dst];
            break;
        case CZ: 
        	if (cpu->registers [REGISTER_CFLAGS] & 0x00000001) 
        	{
        		cpu->registers [REGISTER_STACKPOINTER] -= 4;
            	utils_store (ram, 
            				 cpu->registers [REGISTER_PC], 
            				 cpu->registers[REGISTER_STACKPOINTER], 
            				 32);
                cpu->registers [REGISTER_PC] = long_const;
            }
        	break;
        case CNZ: 
        	if (! (cpu->registers [REGISTER_CFLAGS] & 0x00000001)) 
        	{
        		cpu->registers [REGISTER_STACKPOINTER] -= 4;
            	utils_store (ram, 
            				 cpu->registers [REGISTER_PC], 
            				 cpu->registers[REGISTER_STACKPOINTER], 
            				 32);
                cpu->registers [REGISTER_PC] = long_const;
            }
        	break;
        case CS: 
        	if (cpu->registers [REGISTER_CFLAGS] & 0x00000002)
        	{
        		cpu->registers [REGISTER_STACKPOINTER] -= 4;
            	utils_store (ram, 
            				 cpu->registers [REGISTER_PC], 
            				 cpu->registers[REGISTER_STACKPOINTER], 
            				 32);
                cpu->registers [REGISTER_PC] = long_const;
            }
        	break;
        case CNS: 
        	if (! (cpu->registers [REGISTER_CFLAGS] & 0x00000002))
        	{
        		cpu->registers [REGISTER_STACKPOINTER] -= 4;
            	utils_store (ram, 
            				 cpu->registers [REGISTER_PC], 
            				 cpu->registers[REGISTER_STACKPOINTER], 
            				 32);
                cpu->registers [REGISTER_PC] = long_const;
            }
        	break;
        case DSET: cpu->registers [dst] = long_const + cpu->registers [REGISTER_OFFSET]; break;
        case DCALL: 
        	cpu->registers [REGISTER_STACKPOINTER] -= 4;
            utils_store (ram, 
            			 cpu->registers [REGISTER_PC], 
            			 cpu->registers[REGISTER_STACKPOINTER], 
            			 32);
            cpu->registers [REGISTER_PC] = long_const + cpu->registers [REGISTER_OFFSET]; 
        	break;
        case DJMP: 
			cpu->registers [REGISTER_PC] = long_const + 
                                           cpu->registers [REGISTER_OFFSET]; 
            break;
        case DJR: 
			cpu->registers [REGISTER_PC] = cpu->registers [dst] + 
                                           cpu->registers [REGISTER_OFFSET]; 
            break;
        case DJZ: 
        	if (cpu->registers [REGISTER_CFLAGS] & 0x00000001)
                cpu->registers [REGISTER_PC] = long_const + cpu->registers [REGISTER_OFFSET];
        	break;
        case DJNZ: 
        	if (! (cpu->registers [REGISTER_CFLAGS] & 0x00000001))
                cpu->registers [REGISTER_PC] = long_const + cpu->registers [REGISTER_OFFSET];
        	break;
        case DJS: 
        	if (cpu->registers [REGISTER_CFLAGS] & 0x00000002)
                cpu->registers [REGISTER_PC] = long_const + cpu->registers [REGISTER_OFFSET];
            break;
        case DJNS: 
        	if (! (cpu->registers [REGISTER_CFLAGS] & 0x00000002))
                cpu->registers [REGISTER_PC] = long_const + cpu->registers [REGISTER_OFFSET];
        	break;
        case DRET: 
        	 utils_load (ram, 
            			&cpu->registers [REGISTER_PC], 
            			cpu->registers [REGISTER_STACKPOINTER], 
            			32);
           /*todo: remove... the PC on the stack does not need to be modified on return*/
           /* cpu->registers [REGISTER_PC] += cpu->registers [REGISTER_OFFSET]; */
            cpu->registers [REGISTER_STACKPOINTER] += 4;
        	break;
        case DCR: 
        	cpu->registers [REGISTER_STACKPOINTER] -= 4;
            utils_store (ram, 
            			 cpu->registers [REGISTER_PC], 
            			 cpu->registers[REGISTER_STACKPOINTER], 
            			 32);
            cpu->registers [REGISTER_PC] = cpu->registers [dst] + cpu->registers [REGISTER_OFFSET];
        	break;
        case DCZ: 
        	if (cpu->registers [REGISTER_CFLAGS] & 0x00000001) 
        	{
        		cpu->registers [REGISTER_STACKPOINTER] -= 4;
            	utils_store (ram, 
            				 cpu->registers [REGISTER_PC], 
            				 cpu->registers[REGISTER_STACKPOINTER], 
            				 32);
                cpu->registers [REGISTER_PC] = long_const + cpu->registers [REGISTER_OFFSET];
            }
        	break;
        case DCNZ: 
        	if (! (cpu->registers [REGISTER_CFLAGS] & 0x00000001)) 
        	{
        		cpu->registers [REGISTER_STACKPOINTER] -= 4;
            	utils_store (ram, 
            				 cpu->registers [REGISTER_PC], 
            				 cpu->registers[REGISTER_STACKPOINTER], 
            				 32);
                cpu->registers [REGISTER_PC] = long_const + cpu->registers [REGISTER_OFFSET];
            }
        	break;
        case DCS: 
        	if (cpu->registers [REGISTER_CFLAGS] & 0x00000002)
        	{
        		cpu->registers [REGISTER_STACKPOINTER] -= 4;
            	utils_store (ram, 
            				 cpu->registers [REGISTER_PC], 
            				 cpu->registers[REGISTER_STACKPOINTER], 
            				 32);
                cpu->registers [REGISTER_PC] = long_const + cpu->registers [REGISTER_OFFSET];
            }
        	break;
        case DCNS: 
        	if (! (cpu->registers [REGISTER_CFLAGS] & 0x00000002))
        	{
        		cpu->registers [REGISTER_STACKPOINTER] -= 4;
            	utils_store (ram, 
            				 cpu->registers [REGISTER_PC], 
            				 cpu->registers[REGISTER_STACKPOINTER], 
            				 32);
                cpu->registers [REGISTER_PC] = long_const + cpu->registers [REGISTER_OFFSET];
            }
        	break;
        
        default: cpu->state = STATE_HALT; 
                 printf ("Error: unrecognized opcode: %x\n", opcode);
            break;
	}
	
    /* 
     * if something has occurred to put the PC 
     * in an erroneous state (out of bounds), print debug info
     */
    if (cpu->registers [REGISTER_PC] >= MEMORY_SIZE)
    {
        printf ("Error: PC at 0x%lx\n", 
                (unsigned long)cpu->registers [REGISTER_PC]);
        printf ("Last instruction: \n");
        printf ("0x%x 0x%x 0x%x 0x%x li:0x%x\n", 
										opcode, 
                                        dst, 
                                        src, 
                                        constant,
                                        long_const);
    }

	/* increment */
	cpu->registers [REGISTER_PC] += 4;
    cpu->cycle ++;
}
