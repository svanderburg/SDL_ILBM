/** 
 * Copyright (c) 2012 Sander van der Burg
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software in a
 * product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Sander van der Burg <svanderburg@gmail.com>
 */

#include "window.h"
#include <stdlib.h>

void SDL_ILBM_initWindow(SDL_ILBM_Window *window, const ILBM_Image *image)
{
    window->image = image;
}

SDL_Surface *SDL_ILBM_updateWindowSettings(SDL_ILBM_Window *window, const int fullscreen, const int stretch)
{
    if(stretch)
    {
	window->width = window->image->bitMapHeader->w;
	window->height = window->image->bitMapHeader->h;
    }
    else
    {
	window->width = window->image->bitMapHeader->pageWidth;
	window->height = window->image->bitMapHeader->pageHeight;
    }

    return SDL_SetVideoMode(window->width, window->height, 32, SDL_HWPALETTE | SDL_HWSURFACE | SDL_ANYFORMAT | fullscreen);
}
