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
 * File: emu_sdl.c
 * Description: 
 *--------------------------------------------------
 */

#include "emu_sdl.h"

/*
 *--------------------------------------------------
 * global variables
 *--------------------------------------------------
 */
SDL_Surface    *scr;
SDL_Surface	   *font_surf;
SDL_Surface    *font_fx;
TTF_Font	   *font;
Uint32 			start_tm;
Uint32 			blk_col, wht_col, grn_col, amb_col;
SDL_Color		blk_txt = {0x00, 0x00, 0x00};
SDL_Color		wht_txt	= {0xff, 0xff, 0xff};
SDL_Color       grn_txt = {0x97, 0xff, 0x44};
SDL_Color       amb_txt = {0xff, 0xBE, 0x00};
SDL_Color      *txt_col;
unsigned long   cnt;
unsigned long   draw_cursor;

/*
 *--------------------------------------------------
 * Function: sdl_init
 * Description:	
 *		Initialize SDL content
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
sdl_init(void)
{
	SDL_Init (SDL_INIT_EVERYTHING);
    TTF_Init ();
    scr = SDL_SetVideoMode (9*DISPLAY_COLS+20, 
    						16*DISPLAY_ROWS,
    						32, 
    						SDL_HWSURFACE);
    blk_col = SDL_MapRGB (scr->format, 0x06, 0x06, 0x06);
    wht_col = SDL_MapRGB (scr->format, 0xff, 0xff, 0xff);
    grn_col = SDL_MapRGB (scr->format, 0x00, 0xff, 0x00);
    amb_col = SDL_MapRGB (scr->format, 0xff, 0xbe, 0x00);
    start_tm = 0;
    cnt = 0;
    draw_cursor = 0;
    
    switch (Text_color)
    {
        case MONOGREEN:     txt_col = &grn_txt; break;
        case MONOAMBER:     txt_col = &amb_txt; break;
        default:            txt_col = &wht_txt; break;
    }

    SDL_WM_SetCaption ("C32 Emulator", 0);
    sdl_load_cont ();
}

/*
 *--------------------------------------------------
 * Function: sdl_load_cont
 * Description:	
 *		Load content files (fonts, images, etc.)
 *		-- called by sdl_init ()
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
sdl_load_cont(void)
{
	font 		= TTF_OpenFont (FONT_NAME, FONT_SIZE);
	font_surf 	= TTF_RenderText_Solid (font, "init", wht_txt);
    font_fx     = TTF_RenderText_Solid (font, "init", wht_txt);
}

/*
 *--------------------------------------------------
 * Function: sdl_event_hndl
 * Description:	
 *		Handle all SDL events
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
int
sdl_event_hndl(void)
{
    SDL_Event event;
    int quit_flag		= 1;
    uint32_t keycode	= 0;
    start_tm = SDL_GetTicks ();
    
    while (SDL_PollEvent (&event)) 
    {
        switch (event.type) 
        {
            case SDL_QUIT: quit_flag = 0; break;
            case SDL_KEYDOWN:

				/*
				 *	Assign keypress to keycode.
				 *	Is there a better way to do this?
				 */
            	switch (event.key.keysym.sym) 
            	{
            		case SDLK_a:			keycode = 'a'; break;
            		case SDLK_b:			keycode = 'b'; break;
            		case SDLK_c:			keycode = 'c'; break;
            		case SDLK_d:			keycode = 'd'; break;
            		case SDLK_e:			keycode = 'e'; break;
            		case SDLK_f:			keycode = 'f'; break;
            		case SDLK_g:			keycode = 'g'; break;
            		case SDLK_h:			keycode = 'h'; break;
            		case SDLK_i:			keycode = 'i'; break;
            		case SDLK_j:			keycode = 'j'; break;
            		case SDLK_k:			keycode = 'k'; break;
            		case SDLK_l:			keycode = 'l'; break;
            		case SDLK_m:			keycode = 'm'; break;
            		case SDLK_n:			keycode = 'n'; break;
            		case SDLK_o:			keycode = 'o'; break;
            		case SDLK_p:			keycode = 'p'; break;
            		case SDLK_q:			keycode = 'q'; break;
            		case SDLK_r:			keycode = 'r'; break;
            		case SDLK_s:			keycode = 's'; break;
            		case SDLK_t:			keycode = 't'; break;
            		case SDLK_u:			keycode = 'u'; break;
            		case SDLK_v:			keycode = 'v'; break;
            		case SDLK_w:			keycode = 'w'; break;
            		case SDLK_x:			keycode = 'x'; break;
            		case SDLK_y:			keycode = 'y'; break;
            		case SDLK_z:			keycode = 'z'; break;
            		case SDLK_0:			keycode = '0'; break;
            		case SDLK_1:			keycode = '1'; break;
            		case SDLK_2:			keycode = '2'; break;
            		case SDLK_3:			keycode = '3'; break;
            		case SDLK_4:			keycode = '4'; break;
            		case SDLK_5:			keycode = '5'; break;
            		case SDLK_6:			keycode = '6'; break;
            		case SDLK_7:			keycode = '7'; break;
            		case SDLK_8:			keycode = '8'; break;
            		case SDLK_9:			keycode = '9'; break;
                    case SDLK_SPACE:		keycode = ' '; break;
                    case SDLK_RETURN:	if(Cpu->state == STATE_RUNNING)
                                            keycode = 0x1; 
                                        else
                                            Cpu->state = STATE_RUNNING;
                                                           break;
                    case SDLK_BACKSPACE:	keycode = 0x2; break;
                    case SDLK_UP:			keycode = 0x3; break;
                    case SDLK_DOWN:			keycode = 0x4; break;
                    case SDLK_LEFT:			keycode = 0x5; break;
                    case SDLK_RIGHT:		keycode = 0x6; break;
                    case SDLK_PAGEUP:		keycode = 0x7; break;
                    case SDLK_PAGEDOWN:		keycode = 0x8; break;
                    case SDLK_F2:			keycode = 0x9; break;
                    case SDLK_F4:			keycode = 0xa; break;
                    case SDLK_F1:			Emu_dbg_option = Emu_dbg_option ? 
					                                         0 : 1;
                        break;
                    case SDLK_ESCAPE:       quit_flag = 0;
            		default:				keycode = 0; break;
            	}
            break;
        }
    }
    emu_kb_hndl (&keycode);
    return quit_flag;
}

/*
 *--------------------------------------------------
 * Function: sdl_draw
 * Description:	
 *		clear screen and display
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
sdl_draw(uint8_t *buff)
{
	int r, c;
	uint8_t ln_buf [DISPLAY_COLS];
	
	for (r = 0; r < DISPLAY_ROWS; r++) {
		for (c = 0; c < DISPLAY_COLS; c++) {

			/*
			 * replace all NULL characters with spaces.
			 * sdl_render_text takes a NULL terminated
			 * string... we want the whole line.
			 */
			ln_buf [c] = buff [r*DISPLAY_COLS + c] ? 
                            buff [r*DISPLAY_COLS + c] :
                            ' ';
            /*
             * Replace character with cursor symbol if condition met
             */
            ln_buf [c] = (draw_cursor && Display->cursor.enabled) && 
                         ((r * DISPLAY_COLS + c) == Display->cursor.pos) ?
                            Display->cursor.shape :
                            ln_buf [c]; 
		}
		sdl_render_text (ln_buf, 5, r*15+5);
	}

    /*
     * Draw the hardware cursor, if enabled.
     */
    if (Display->cursor.enabled &&
        (cnt % 20 == 0))
    {
        draw_cursor = draw_cursor ? 0 : 1;
    }
    
    cnt++;
}

/*
 *--------------------------------------------------
 * Function: sdl_draw_info
 * Description:	
 *		draws the program title and some information
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
sdl_draw_info(void)
{
    char buffer [64];
    int x = 160, y = 100;

    sdl_render_text ((uint8_t *)"C32-EMU", x, y);
    sdl_render_text ((uint8_t *)"An emulator for the Custom 32-bit RISC architecture", x, y + 10);
    sdl_render_text ((uint8_t *)"Copyright (C) 2013, Joe Mruz", x, y + 20);
    sdl_render_text ((uint8_t *)"v"EMU_VERSION, x, y + 40);
    sdl_render_text ((uint8_t *)"Options:", x, y + 120);
    sdl_render_text ((uint8_t *)"[ENTER] Start emulator", x, y + 140);
    sdl_render_text ((uint8_t *)"[F1] Toggle live register dump", x, y + 160);
    sdl_render_text ((uint8_t *)"[ESC] Quit", x, y + 180);

    memset (buffer, 0, sizeof (buffer));
    sprintf (buffer, "Disk:    %s", Disk_filn);
    sdl_render_text ((uint8_t *)buffer, x, y + 300);

    memset (buffer, 0, sizeof (buffer));
    sprintf (buffer, "ROM:     %s", Rom_filn);
    sdl_render_text ((uint8_t *)buffer, x, y + 320);
}

/*
 *--------------------------------------------------
 * Function: sdl_flip
 * Description:	
 *		Wraps the SDL call for screen flip
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
sdl_flip(void)
{
	SDL_Flip (scr);
}

/*
 *--------------------------------------------------
 * Function: sdl_render_text
 * Description:	
 *		Helper method to render text to location
 *		on screen.
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
sdl_render_text(uint8_t *buff, int x, int y)
{
    int radius    = 2;    /* "Bloom" radius;      0-???, Default: 2 */
    int intensity = 16;   /* "Bloom" intensity;   0-255, Default: 16 */
    int fga       = 220;  /* Foreground alpha;    0-255, Default: 220 */

    if(font_surf) SDL_FreeSurface(font_surf);
    if(font_fx) SDL_FreeSurface(font_fx);
	
    font_surf = TTF_RenderText_Blended (font, (void *)buff, *txt_col);
    font_fx   = TTF_RenderText_Solid (font, (void *)buff, *txt_col);
	
    /*
     * This is sort of a cheap way to get a slight bloom effect on
     * on the text. Of course we have to draw a lot more text this way...
     */
    SDL_SetAlpha (font_fx, SDL_SRCALPHA, intensity);
    SDL_SetAlpha (font_surf, SDL_SRCALPHA, fga);
    SDL_Surface* hw_alpha = SDL_DisplayFormatAlpha (font_fx);
    SDL_Surface* fg_alpha = SDL_DisplayFormatAlpha (font_surf);   
    utils_apply_surface (x, y, fg_alpha, scr);
    
    utils_apply_surface (x + radius, y + radius, hw_alpha, scr);
    utils_apply_surface (x + radius, y, hw_alpha, scr);
    utils_apply_surface (x, y + radius, hw_alpha, scr);
    utils_apply_surface (x, y - radius, hw_alpha, scr);
    utils_apply_surface (x - radius, y, hw_alpha, scr);
    utils_apply_surface (x, y - radius, hw_alpha, scr);
    utils_apply_surface (x - radius, y - radius, hw_alpha, scr);
    utils_apply_surface (x - radius, y + radius, hw_alpha, scr);
    utils_apply_surface (x + radius, y - radius, hw_alpha, scr);
    SDL_FreeSurface (hw_alpha);
    SDL_FreeSurface (fg_alpha);
}

/*
 *--------------------------------------------------
 * Function: sdl_render_text_attr
 * Description:	
 *		Helper method to render text to location
 *		on screen with certain attributes.
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
sdl_render_text_attr(uint8_t *buff, int x, int y, TTF_Font *font, SDL_Color *color)
{
    if(font_surf) SDL_FreeSurface(font_surf);
	font_surf = TTF_RenderText_Solid (font, (void *)buff, *color);
	utils_apply_surface (x, y, font_surf, scr);
}

/*
 *--------------------------------------------------
 * Function: sdl_render_box
 * Description:	
 *		Helper method to render a black box for
 *		debug information to be written on.
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void sdl_render_box(int x, int y, int w, int h)
{
	SDL_Rect r = {x, y, w, h};
	SDL_FillRect (scr, &r, blk_col);
}

/*
 *--------------------------------------------------
 * Function: sdl_clear
 * Description:	
 *		Clears the screen
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
sdl_clear(void)
{
	SDL_FillRect (scr, &scr->clip_rect, blk_col);
}

/*
 *--------------------------------------------------
 * Function: sdl_sleep
 * Description:	
 *		SDL sleep function
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
sdl_sleep(void)
{
	if (1000/FPS > SDL_GetTicks() - start_tm) 
	{
        SDL_Delay (1000/FPS - (SDL_GetTicks () - start_tm));
    }
}

/*
 *--------------------------------------------------
 * Function: sdl_quit
 * Description:	
 *		signal an SDL quit -- cleanup
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
sdl_quit(void)
{
	SDL_FreeSurface (scr);
	SDL_FreeSurface (font_surf);
    SDL_FreeSurface (font_fx);
	TTF_CloseFont (font);
	TTF_Quit ();
	SDL_Quit ();
}
