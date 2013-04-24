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
 * File: emu_utils.c
 * Description:
 *		misc helper functions
 *--------------------------------------------------
 */

#include "emu_utils.h"
#include "stdio.h"

/*
 *--------------------------------------------------
 * Function: util_apply_surface
 * Description:	
 *		blit source surface onto destination at
 *		location (x, y)
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
utils_apply_surface(int x, int y, SDL_Surface *src, SDL_Surface *dst)
{ 
	SDL_Rect offset;
	offset.x = x; 
	offset.y = y;
	SDL_BlitSurface (src, NULL, dst, &offset);
}

/*
 *--------------------------------------------------
 * Function: utils_store
 * Description:	
 *		Copy 1 to 4 bytes to RAM at address.
 *		Note, little endian.
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
utils_store(ram_t *ram, uint32_t word, uint32_t adr, uint16_t bits)
{
    if (adr >= MEMORY_SIZE)
        return;

	/* little endian */
    switch (bits)
    {
        case 32:
            ram->address [adr]      = (uint8_t)(word) & 0xFF;
            ram->address [adr + 1]  = (uint8_t)(word >> 8) & 0xFF;
            ram->address [adr + 2]  = (uint8_t)(word >> 16) & 0xFF;
            ram->address [adr + 3]  = (uint8_t)(word >> 24)& 0xFF; 
            break;
        case 16:
            ram->address [adr]      = (uint8_t)(word) & 0xFF;
            ram->address [adr + 1]  = (uint8_t)(word >> 8) & 0xFF; 
            break;
        case 8:
            ram->address [adr]      = (uint8_t)(word) & 0xFF;
            break;
        default: 
            return;
    }
}

/*
 *--------------------------------------------------
 * Function: utils_load
 * Description:	
 *		Retrieve 1 to 4 bytes from RAM
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
utils_load(ram_t *ram, uint32_t *dst, uint32_t adr, uint16_t bits)
{
    if(adr >= MEMORY_SIZE)
        return;

	//little endian
    switch (bits)
    {
        case 32: 
            *dst = ram->address[adr] | 
            		(ram->address[adr+1] << 8) | 
            		(ram->address[adr+2] << 16) | 
            		(ram->address[adr+3] << 24);
            break;
        case 16:
            *dst = (ram->address [adr]) |
                   (ram->address [adr+1] << 8);
            break;
        case 8:
            *dst = ram->address[adr];
            break;
        default:
            return;
    }
}

/*
 *--------------------------------------------------
 * Function: utils_cpy_chunk
 * Description:	
 *		Copy a chunk of RAM to another location
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
utils_cpy_chunk(ram_t *ram, uint32_t dst, uint32_t src, uint32_t bytes)
{
    unsigned int i;

    for (i = 0; i < bytes; i++)
    {
        ram->address [dst + i] = ram->address [src + i];
    }
}

/*
 *--------------------------------------------------
 * Function: utils_compare
 * Description:	
 *		Compares two 4 byte values by setting flags
 *		in the CFLAGS register.
 *		
 *		low bits -> high bits
 *		(1) zero flag, (2) sign flag, (3-32) unassigned
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
utils_compare(uint32_t *reg_cmp, uint32_t w1, uint32_t w2)
{
    int diff;

    *reg_cmp = 0;
    diff = (int)w1 - (int)w2;
    
    if(!diff)       *reg_cmp    = 1;
    if(diff < 0)    *reg_cmp    = 2;
}
