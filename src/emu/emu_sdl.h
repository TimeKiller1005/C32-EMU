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
 * File: emu_sdl.h
 * Description:
 *--------------------------------------------------
 */

#ifndef _EMU_SDL_H_
#define _EMU_SDL_H_

#include <stdint.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include "emu.h"
#include "emu_utils.h"

/*
 *--------------------------------------------------
 * constants
 *--------------------------------------------------
 */
#define FONT_NAME 	"res/Perfect DOS VGA 437.ttf"/*"res/ArcadeClassic.ttf"*//*"res/DejaVuSansMono.ttf"*/
#define FONT_SIZE   16 /* default for Perfect DOS: 16 */
#define FPS			60

/*
 *--------------------------------------------------
 * functions
 *--------------------------------------------------
 */
void    sdl_init(void);
void    sdl_load_cont(void);
int     sdl_event_hndl(void);
void    sdl_draw(uint8_t *buffer);
void    sdl_draw_info(void);
void	sdl_render_text(uint8_t *buffer, int x, int y);
void	sdl_render_text_attr(uint8_t *buff, int x, int y, TTF_Font *font, SDL_Color *color);
void	sdl_render_box(int x, int y, int w, int h);
void	sdl_flip(void);
void    sdl_clear(void);
void    sdl_sleep(void);
void    sdl_quit(void);

#endif  /* _EMU_SDL_H_ */
