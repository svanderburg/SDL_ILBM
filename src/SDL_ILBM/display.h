/*
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

#ifndef __SDL_ILBM_DISPLAY_H
#define __SDL_ILBM_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_ILBM_Display SDL_ILBM_Display;

#include "image.h"

struct SDL_ILBM_Display
{
    const SDL_ILBM_Image *image;
    int width;
    int height;
    SDL_Surface *blitSurface;
    int mustFreeBlitSurface;
};

int SDL_ILBM_initDisplay(SDL_ILBM_Display *display, const SDL_ILBM_Image *image, const int stretch);

SDL_ILBM_Display *SDL_ILBM_createDisplay(const SDL_ILBM_Image *image, const int stretch);

void SDL_ILBM_destroyDisplay(SDL_ILBM_Display *display);

void SDL_ILBM_freeDisplay(SDL_ILBM_Display *display);

SDL_Window *SDL_ILBM_createWindow(const char *title, int x, int y, const SDL_ILBM_Display *display, Uint32 flags);

int SDL_ILBM_renderSetLogicalSize(SDL_Renderer *renderer, const SDL_ILBM_Display *display);

SDL_Texture *SDL_ILBM_createTexture(SDL_Renderer *renderer, Uint32 format, int access, const SDL_ILBM_Display *display);

int SDL_ILBM_blitDisplayToTexture(SDL_ILBM_Display *display, Uint32 format, void *pixels, int pitch);

int SDL_ILBM_renderCopy(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, const SDL_ILBM_Display *display);

#ifdef __cplusplus
}
#endif

#endif
