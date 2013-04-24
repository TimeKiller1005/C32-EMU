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
 * File: emu_utils.h
 * Description:
 *		misc helper functions
 *--------------------------------------------------
 */

#ifndef _EMU_UTILS_H_
#define _EMU_UTILS_H_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "emu.h"

void utils_load(ram_t *ram, uint32_t *dst, uint32_t adr, uint16_t bits);
void utils_store(ram_t *ram, uint32_t word, uint32_t adr, uint16_t bits);
void utils_cpy_chunk(ram_t *ram, uint32_t dst, uint32_t src, uint32_t bytes);
void utils_apply_surface(int x, int y, SDL_Surface *src, SDL_Surface *dst);
void utils_compare(uint32_t *reg_cmp, uint32_t w1, uint32_t w2);

#endif	/* _EMU_UTILS_H_ */
