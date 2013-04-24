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
 * File: emu_ram.c
 * Description: Functionality of RAM object
 *--------------------------------------------------
 */

#include <string.h>
#include "emu.h"

/*
 *--------------------------------------------------
 * Function: ram_init
 * Description:	
 *		Initialize the RAM object
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
ram_init(ram_t *ram)
{
	memset (ram->address, 0, sizeof (ram->address));
}
