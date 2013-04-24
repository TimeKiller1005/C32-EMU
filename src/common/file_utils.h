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
 * Description:
 *  File read/write functions
 *
 * author: joe mruz
 *--------------------------------------------------
 */
#ifndef _FILE_UTILS_H_
#define _FILE_UTILS_H_

#include <stddef.h>

/*
 *--------------------------------------------------
 * Functions
 *--------------------------------------------------
 */
int file_utils_read(char *buffer, size_t size, const char *filename);
int file_utils_write(char *buffer, size_t size, const char *filename);

#endif	/* _FILE_UTILS_H_ */
