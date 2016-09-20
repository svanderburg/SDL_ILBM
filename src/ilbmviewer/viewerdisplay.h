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

#ifndef __SDL_ILBM_VIEWERDISPLAY_H
#define __SDL_ILBM_VIEWERDISPLAY_H
#include <SDL.h>
#include "display.h"
#include "image.h"

typedef struct
{
    SDL_ILBM_Display display;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    int offsetX, offsetY;
}
SDL_ILBM_ViewerDisplay;

int SDL_ILBM_initViewerDisplay(SDL_ILBM_ViewerDisplay *viewerDisplay, SDL_ILBM_Image *image, const int stretch, const int fullscreen);

void SDL_ILBM_destroyViewerDisplay(SDL_ILBM_ViewerDisplay *viewerDisplay);

int SDL_ILBM_renderTexture(SDL_ILBM_ViewerDisplay *viewerDisplay);

int SDL_ILBM_scrollWindowLeft(SDL_ILBM_ViewerDisplay *viewerDisplay);

int SDL_ILBM_scrollWindowRight(SDL_ILBM_ViewerDisplay *viewerDisplay);

int SDL_ILBM_scrollWindowUp(SDL_ILBM_ViewerDisplay *viewerDisplay);

int SDL_ILBM_scrollWindowDown(SDL_ILBM_ViewerDisplay *viewerDisplay);

#endif
