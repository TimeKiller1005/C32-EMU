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
 * File: emu_clk.c
 * Description: Hardware clock implementation
 *--------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "emu.h"
#include "emu_utils.h"

/*
 *--------------------------------------------------
 * Constants
 *--------------------------------------------------
 */
#define CLK_CODE_QUANTUM    (0x1)
#define CLK_CODE_ADDRESS    (0x2)
#define CLK_CODE_TIME       (0x3)

enum clk_states
{
    CLK_READ_QUANTUM,
    CLK_READ_ADDRESS,
    CLK_READ_REQUEST
};

/*
 *--------------------------------------------------
 * Functions
 *--------------------------------------------------
 */

/*
 *--------------------------------------------------
 * Function: clk_init
 * Description:	
 *		initialize clock object fields
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
clk_init(clk_t *clk)
{
    memset (clk, 0, sizeof (clk));
    clk->state = CLK_READ_REQUEST;
}

/*
 *--------------------------------------------------
 * Function: clk_rd_port
 * Description:	
 *		read from the port
 *      (state machine)
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
clk_rd_port(clk_t *clk, uint32_t *dat)
{
    if (! *dat) return;

    switch (clk->state)
    {
        case CLK_READ_QUANTUM: 
                clk->quantum    = *dat;
                clk->state      = CLK_READ_REQUEST;
            break;
        case CLK_READ_ADDRESS: 
                clk->address    = *dat;
                clk->state      = CLK_READ_REQUEST;
            break;
        case CLK_READ_REQUEST: 
                if (*dat == CLK_CODE_QUANTUM) clk->state    = CLK_READ_QUANTUM;
                if (*dat == CLK_CODE_ADDRESS) clk->state    = CLK_READ_ADDRESS;
                if (*dat == CLK_CODE_TIME)    clk_get_tv_usec (dat);
            break;
        default: clk->state = CLK_READ_REQUEST; break;
    }
}

/*
 *--------------------------------------------------
 * Function: clk_chk_tmr
 * Description:	
 *		check the timer, set cpu program counter
 *		if time quantum is up.
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
clk_chk_tmr(clk_t *clk, cpu_t *cpu, ram_t *ram)
{
    uint32_t tv_usec;
    if (!clk->quantum || !clk->address) return;
    clk_get_tv_usec (&tv_usec);

    /* if we have reached our quantum (to the microsecond) */
    if (tv_usec - clk->prv_tm >= (clk->quantum * 1000))
    {
        /* Perform a "CALL" and jump to clk->address */
        cpu->registers [REGISTER_STACKPOINTER] -= 4;
        utils_store (ram, 
                     cpu->registers [REGISTER_PC], 
                     cpu->registers[REGISTER_STACKPOINTER], 
                     32);
        cpu->registers [REGISTER_PC] = clk->address;

        /* Update stored time of last jump */
        clk->prv_tm = tv_usec;
    }
}

/*
 *--------------------------------------------------
 * Function: clk_get_usec
 * Description:
 *      get the current time value in microseconds
 *      from a particular epoch.
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
clk_get_tv_usec(uint32_t *tv_usec)
{
    struct timeval tv;
    gettimeofday (&tv, 0);
    *tv_usec = (uint32_t) tv.tv_usec;
}
