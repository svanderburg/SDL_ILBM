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

void SDL_ILBM_initWindow(SDL_ILBM_Window *window, const char *title, const ILBM_Image *image, const amiVideo_Screen *screen, SDL_Surface *pictureSurface, const int fullscreen, const int stretch, const unsigned int lowresPixelScaleFactor)
{
    window->title = title;
    window->window = NULL;
    window->renderer = NULL;
    window->image = image;
    window->screen = screen;
    window->pictureSurface = pictureSurface;
    window->windowTexture = NULL;
    window->fullscreen = fullscreen;
    window->stretch = stretch;
    window->lowresPixelScaleFactor = lowresPixelScaleFactor;
    window->xOffset = 0;
    window->yOffset = 0;
    
    SDL_ILBM_updateWindowSettings(window);
}

void SDL_ILBM_destroyWindow(SDL_ILBM_Window *window)
{
    SDL_DestroyTexture(window->windowTexture);
    SDL_DestroyRenderer(window->renderer);
    SDL_DestroyWindow(window->window);
}

void SDL_ILBM_updateWindowSettings(SDL_ILBM_Window *window)
{
    Uint32 fullScreenFlag;
    
    /* Determine window settings */
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
    
    if(window->fullscreen)
	fullScreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    else
	fullScreenFlag = 0;
    
    /* Discard old window */
    SDL_ILBM_destroyWindow(window);
    
    /* Create a new window and renderer */
    window->window = SDL_CreateWindow(window->title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window->width, window->height, fullScreenFlag);
    
    if(window->window == NULL)
    {
	fprintf(stderr, "Cannot create window: %s\n", SDL_GetError());
	return;
    }
    
    window->renderer = SDL_CreateRenderer(window->window, -1, 0);
    
    if(window->renderer == NULL)
    {
	fprintf(stderr, "Cannot create renderer: %s\n", SDL_GetError());
	return;
    }
    
    /* Set scaling options */
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(window->renderer, window->width, window->height);
    
    /* Clear the screen */
    SDL_SetRenderDrawColor(window->renderer, 0, 0, 0, 255);
    SDL_RenderClear(window->renderer);
    
    /* Configure window texture */
    window->windowTexture = SDL_CreateTextureFromSurface(window->renderer, window->pictureSurface);
    
    if(window->windowTexture == NULL)
	fprintf(stderr, "Cannot create texture: %s\n", SDL_GetError());
}

int SDL_ILBM_blitPictureInWindow(SDL_ILBM_Window *window)
{
    /* Compose position and clipping rectange */
    int width = window->pictureSurface->w - window->xOffset;
    int height = window->pictureSurface->h - window->yOffset;
    SDL_Rect srcrect;
    
    if(width > window->width)
        width = window->width;
    
    if(height > window->height)
        height = window->height;
    
    srcrect.x = window->xOffset;
    srcrect.y = window->yOffset;
    srcrect.w = width;
    srcrect.h = height;
    
    /* Render the texture */
    if(SDL_RenderCopy(window->renderer, window->windowTexture, &srcrect, NULL) == 0)
	return TRUE;
    else
    {
	fprintf(stderr, "Render copy failed: %s\n", SDL_GetError());
	return FALSE;
    }
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
