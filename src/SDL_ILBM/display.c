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

#include "display.h"
#include <stdlib.h>
#include <SDL.h>

int SDL_ILBM_initDisplay(SDL_ILBM_Display *display, const SDL_ILBM_Image *image, const int stretch)
{
    display->image = image;

    /* Determine the appropriate dimensions of the display */
    if(stretch)
    {
        display->width = image->surface->w;
        display->height = image->surface->h;
    }
    else
    {
        if(image->lowresPixelScaleFactor == 1)
        {
            display->width = image->image->bitMapHeader->pageWidth;
            display->height = image->image->bitMapHeader->pageHeight;
        }
        else
        {
            display->width = amiVideo_calculateCorrectedWidth(image->lowresPixelScaleFactor, image->image->bitMapHeader->pageWidth, image->screen.viewportMode);
            display->height = amiVideo_calculateCorrectedHeight(image->lowresPixelScaleFactor, image->image->bitMapHeader->pageHeight, image->screen.viewportMode);
        }
    }

    /* Allocate a conversion surface, if needed */

    if(image->format == SDL_ILBM_CHUNKY_FORMAT)
    {
        display->mustFreeBlitSurface = TRUE;
        display->blitSurface = SDL_CreateRGBSurface(0, image->surface->w, image->surface->h, 32, 0, 0, 0, 0);

        return (display->blitSurface != NULL);
    }
    else
    {
        display->mustFreeBlitSurface = FALSE;
        display->blitSurface = image->surface;

        return TRUE;
    }
}

SDL_ILBM_Display *SDL_ILBM_createDisplay(const SDL_ILBM_Image *image, const int stretch)
{
    SDL_ILBM_Display *display = (SDL_ILBM_Display*)malloc(sizeof(SDL_ILBM_Image));

    if(display != NULL)
    {
        if(!SDL_ILBM_initDisplay(display, image, stretch))
        {
            SDL_ILBM_freeDisplay(display);
            return NULL;
        }
    }

    return display;
}

void SDL_ILBM_destroyDisplay(SDL_ILBM_Display *display)
{
    if(display->mustFreeBlitSurface)
        SDL_FreeSurface(display->blitSurface);
}

void SDL_ILBM_freeDisplay(SDL_ILBM_Display *display)
{
    if(display != NULL)
    {
        SDL_ILBM_destroyDisplay(display);
        free(display);
    }
}

SDL_Window *SDL_ILBM_createWindow(const char *title, int x, int y, const SDL_ILBM_Display *display, Uint32 flags)
{
    return SDL_CreateWindow(title, x, y, display->width, display->height, flags);
}

int SDL_ILBM_renderSetLogicalSize(SDL_Renderer *renderer, const SDL_ILBM_Display *display)
{
    return SDL_RenderSetLogicalSize(renderer, display->width, display->height);
}

SDL_Texture *SDL_ILBM_createTexture(SDL_Renderer *renderer, Uint32 format, int access, const SDL_ILBM_Display *display)
{
    return SDL_CreateTexture(renderer, format, access, display->blitSurface->w, display->blitSurface->h);
}

amiVideo_Bool SDL_ILBM_blitDisplayToTexture(SDL_ILBM_Display *display, Uint32 format, void *pixels, int pitch)
{
    if(display->mustFreeBlitSurface)
    {
        if(SDL_BlitSurface(display->image->surface, NULL, display->blitSurface, NULL) < 0) /* Convert the picture surface's format to the window surface format by blitting it */
            return FALSE;
    }

    /* Transfer and convert the pixels to the texture */

    if(SDL_ConvertPixels(display->blitSurface->w, display->blitSurface->h, display->blitSurface->format->format, display->blitSurface->pixels, display->blitSurface->pitch, format, pixels, pitch) < 0)
        return FALSE;
    else
        return TRUE;
}

int SDL_ILBM_renderCopy(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, const SDL_ILBM_Display *display)
{
    SDL_Rect srcrect;
    srcrect.x = x;
    srcrect.y = y;
    srcrect.w = display->width;
    srcrect.h = display->height;

    return SDL_RenderCopy(renderer, texture, &srcrect, NULL);
}
