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

#include <string.h>
#include <time.h>
#include "emu.h"

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
                if (*dat == CLK_CODE_TIME)    *dat          = 0; /* todo: set this to the current time */
            break;
        default: clk->state = CLK_READ_REQUEST; break;
    }

    *dat = 0;
}

/*
 *--------------------------------------------------
 * Function: clk_chk_tmr
 * Description:	
 *		check the timer, set cpu program counter
 *		if time.
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
clk_chk_tmr(clk_t *clk, cpu_t *cpu, ram_t *ram)
{
    if (!clk->quantum || !clk->address) return;

}
