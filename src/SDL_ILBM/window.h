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

#ifndef __SDL_ILBM_WINDOW_H
#define __SDL_ILBM_WINDOW_H

typedef struct SDL_ILBM_Window SDL_ILBM_Window;

#include <SDL.h>
#include <libilbm/ilbmimage.h>
#include <libamivideo/screen.h>

struct SDL_ILBM_Window
{
    const char *title;
    const ILBM_Image *image;
    const amiVideo_Screen *screen;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *pictureSurface;
    SDL_Texture *windowTexture;
    int width;
    int height;
    Sint16 xOffset, yOffset;
    int fullscreen;
    int stretch;
    unsigned int lowresPixelScaleFactor;
};

void SDL_ILBM_initWindow(SDL_ILBM_Window *window, const char *title, const ILBM_Image *image, const amiVideo_Screen *screen, SDL_Surface *pictureSurface, const int fullscreen, const int stretch, const unsigned int lowresPixelScaleFactor);

void SDL_ILBM_destroyWindow(SDL_ILBM_Window *window);

void SDL_ILBM_updateWindowSettings(SDL_ILBM_Window *window);

int SDL_ILBM_blitPictureInWindow(SDL_ILBM_Window *window);

int SDL_ILBM_toggleWindowFullscreen(SDL_ILBM_Window *window);

int SDL_ILBM_toggleWindowStretch(SDL_ILBM_Window *window);

int SDL_ILBM_scrollWindowLeft(SDL_ILBM_Window *window);

int SDL_ILBM_scrollWindowRight(SDL_ILBM_Window *window);

int SDL_ILBM_scrollWindowUp(SDL_ILBM_Window *window);

int SDL_ILBM_scrollWindowDown(SDL_ILBM_Window *window);

#endif
