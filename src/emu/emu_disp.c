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
 * File: emu_disp.c
 * Description: Simulates functionality of display device
 *--------------------------------------------------
 */

#include <string.h>
#include "emu.h"

/*
 *--------------------------------------------------
 * constants
 *--------------------------------------------------
 */

/*
 * Highest and lowest addresses cannot be used to indicate 
 * the video memory map, so we can use these values as 
 * special purpose codes.
 */
#define DISP_CLEAR_CODE		0x1
#define DISP_UPDATE_CODE    0x2
#define DISP_RESET_CODE     0xFFFFF
#define DISP_HWCURSOR_ON    0xFFFFE
#define DISP_HWCURSOR_OFF   0xFFFFD
#define DISP_HWCURSOR_POS   0xFFFFC
#define DISP_HWCURSOR_SHAPE 0xFFFFB

/*
 *--------------------------------------------------
 * objects
 *--------------------------------------------------
 */
enum disp_state
{
	DISP_READ_CHAR,
	DISP_READ_ADR,
    DISP_READ_CURSOR_POS,
    DISP_READ_CURSOR_SHAPE
};

struct disp_packet_s
{
	uint8_t ascii;
	uint8_t pos;
	uint16_t pad;
};

/*
 *--------------------------------------------------
 * functions
 *--------------------------------------------------
 */

/*
 *--------------------------------------------------
 * Function: disp_init
 * Description:	
 *		Initialize the display object
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
disp_init(disp_t* disp)
{
	memset (disp->buffer, 0x0, sizeof (disp->buffer));
	
	disp->state             = DISP_READ_ADR;
    disp->buffer_map_adr    = 0;
    disp->cursor.pos        = 0;
    disp->cursor.shape      = 0xdb;
    disp->cursor.enabled    = 0;
}

/*
 *--------------------------------------------------
 * Function: disp_rd_port
 * Description:	
 *      Display device state machine.
 *		Read/update the simulated display I/O port.
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
disp_rd_port(disp_t *disp, uint32_t *dat, ram_t *ram)
{
    if (!(*dat))
	{
		return;
	}
    
    if (disp->state == DISP_READ_CHAR)
    {
        switch (*dat)
        {
            case DISP_CLEAR_CODE: 
                memset (disp->buffer, 0, sizeof (disp->buffer));
                break;
            case DISP_UPDATE_CODE:
                disp_rd_adr_map (disp, ram);
                break;
            case DISP_RESET_CODE: 
                disp->state = DISP_READ_ADR;
                break;
            case DISP_HWCURSOR_ON: 
                disp->cursor.enabled = 1; 
                break;
            case DISP_HWCURSOR_OFF: 
                disp->cursor.enabled = 0;
                break;
            case DISP_HWCURSOR_POS: 
                disp->state = DISP_READ_CURSOR_POS;
                break;
            case DISP_HWCURSOR_SHAPE: 
                disp->state = DISP_READ_CURSOR_SHAPE;
                break;
        }
    }
    else if (disp->state == DISP_READ_ADR)
    {
        if (*dat)
        {
            disp->buffer_map_adr = *dat;
            disp->state = DISP_READ_CHAR;
        }
    }
    else if (disp->state == DISP_READ_CURSOR_POS)
    {
        if (*dat)
        {
            disp->cursor.pos = *dat;
            disp->state = DISP_READ_CHAR;
        }
    }
    else if (disp->state == DISP_READ_CURSOR_SHAPE)
    {
        if (*dat)
        {
            disp->cursor.shape = *dat;
            disp->state = DISP_READ_CHAR;
        }
    }
    
    *dat = 0;
}

/*
 *--------------------------------------------------
 * Function: disp_rd_adr_map
 * Description:	
 *		Read video memory from RAM and fill
 *		internal memory.
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
disp_rd_adr_map(disp_t *disp, ram_t *ram)
{
	unsigned int i;
	
    if (! disp->buffer_map_adr)
	{
		return;
	}
	
	for (i = 0; i < DISPLAY_BUFFER_SIZE; i++) 
	{
		disp->buffer [i] = ram->address [(disp->buffer_map_adr) + i];
	}
}
