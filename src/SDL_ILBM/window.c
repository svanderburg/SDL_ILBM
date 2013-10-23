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

#include "window.h"
#include <stdlib.h>

void SDL_ILBM_initWindow(SDL_ILBM_Window *window, const ILBM_Image *image, const amiVideo_Screen *screen, SDL_Surface *pictureSurface, const int fullscreen, const int stretch, const unsigned int lowresPixelScaleFactor)
{
    window->image = image;
    window->screen = screen;
    window->pictureSurface = pictureSurface;
    window->fullscreen = fullscreen;
    window->stretch = stretch;
    window->lowresPixelScaleFactor = lowresPixelScaleFactor;
    window->xOffset = 0;
    window->yOffset = 0;
    
    SDL_ILBM_updateWindowSettings(window);
}

void SDL_ILBM_updateWindowSettings(SDL_ILBM_Window *window)
{
    Uint32 fullscreenFlag;
    
    if(window->fullscreen)
	fullscreenFlag = SDL_FULLSCREEN;
    else
	fullscreenFlag = 0;
    
    if(window->stretch)
    {
	window->width = window->image->bitMapHeader->w;
	window->height = window->image->bitMapHeader->h;
    }
    else
    {
        window->width = window->image->bitMapHeader->pageWidth;
        window->height = window->image->bitMapHeader->pageHeight;
    }
    
    if(window->lowresPixelScaleFactor > 1)
    {
        window->width = amiVideo_calculateCorrectedWidth(window->lowresPixelScaleFactor, window->width, window->screen->viewportMode);
        window->height = amiVideo_calculateCorrectedHeight(window->lowresPixelScaleFactor, window->height, window->screen->viewportMode);
    }
    
    window->windowSurface = SDL_SetVideoMode(window->width, window->height, 32, SDL_HWPALETTE | SDL_HWSURFACE | SDL_ANYFORMAT | fullscreenFlag);
}

int SDL_ILBM_blitPictureInWindow(SDL_ILBM_Window *window)
{
    SDL_Rect dstrect;
    dstrect.x = -window->xOffset;
    dstrect.y = -window->yOffset;
    
    if(SDL_BlitSurface(window->pictureSurface, NULL, window->windowSurface, &dstrect) == -1)
	return FALSE;
    else
	return TRUE;
}

int SDL_ILBM_toggleWindowFullscreen(SDL_ILBM_Window *window)
{
    window->fullscreen = !window->fullscreen;
    SDL_ILBM_updateWindowSettings(window);
    return SDL_ILBM_blitPictureInWindow(window);
}

int SDL_ILBM_toggleWindowStretch(SDL_ILBM_Window *window)
{
    window->stretch = !window->stretch;
    SDL_ILBM_updateWindowSettings(window);
    return SDL_ILBM_blitPictureInWindow(window);
}

int SDL_ILBM_scrollWindowLeft(SDL_ILBM_Window *window)
{
    if(window->xOffset > 0)
	window->xOffset--;
    
    return SDL_ILBM_blitPictureInWindow(window);
}

int SDL_ILBM_scrollWindowRight(SDL_ILBM_Window *window)
{
    if(window->xOffset < window->pictureSurface->w - window->width)
	window->xOffset++;
	
    return SDL_ILBM_blitPictureInWindow(window);
}

int SDL_ILBM_scrollWindowUp(SDL_ILBM_Window *window)
{
    if(window->yOffset > 0)
	window->yOffset--;
    
    return SDL_ILBM_blitPictureInWindow(window);
}

int SDL_ILBM_scrollWindowDown(SDL_ILBM_Window *window)
{
    if(window->yOffset < window->pictureSurface->h - window->height)
	window->yOffset++;

    return SDL_ILBM_blitPictureInWindow(window);
}
