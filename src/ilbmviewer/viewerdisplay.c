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

#include "viewerdisplay.h"

int SDL_ILBM_initViewerDisplay(SDL_ILBM_ViewerDisplay *viewerDisplay, SDL_ILBM_Image *image, const int stretch, const int fullscreen)
{
    Uint32 fullScreenFlag;

    /* Set up a display from the image and the display settings */
    SDL_ILBM_initDisplay(&viewerDisplay->display, image, stretch);

    /* Configure the fullscreen flag if the fullscreen setting has been provided */
    if(fullscreen)
        fullScreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    else
        fullScreenFlag = 0;

    /* Initialize offset coordinates */
    viewerDisplay->offsetX = 0;
    viewerDisplay->offsetY = 0;

    /* Create a SDL window */
    viewerDisplay->window = SDL_ILBM_createWindow("SDL ILBM Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, &viewerDisplay->display, fullScreenFlag);

    if(viewerDisplay->window == NULL)
    {
        fprintf(stderr, "Cannot create window!\n");
        return FALSE;
    }

    /* Create a renderer */
    viewerDisplay->renderer = SDL_CreateRenderer(viewerDisplay->window, -1, 0);

    if(viewerDisplay->renderer == NULL)
    {
        fprintf(stderr, "Cannot create renderer!\n");
        SDL_ILBM_destroyViewerDisplay(viewerDisplay);
        return FALSE;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_ILBM_renderSetLogicalSize(viewerDisplay->renderer, &viewerDisplay->display);

    /* Clear the renderer with a black color */
    SDL_SetRenderDrawColor(viewerDisplay->renderer, 0, 0, 0, 255);
    SDL_RenderClear(viewerDisplay->renderer);

    /* Create a texture */
    viewerDisplay->texture = SDL_ILBM_createTexture(viewerDisplay->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, &viewerDisplay->display);

    if(viewerDisplay->texture == NULL)
    {
        fprintf(stderr, "Cannot create texture!\n");
        SDL_ILBM_destroyViewerDisplay(viewerDisplay);
        return FALSE;
    }

    /* Render the texture */
    return SDL_ILBM_renderTexture(viewerDisplay);
}

void SDL_ILBM_destroyViewerDisplay(SDL_ILBM_ViewerDisplay *viewerDisplay)
{
    if(viewerDisplay != NULL)
    {
        SDL_ILBM_destroyDisplay(&viewerDisplay->display);
        SDL_DestroyTexture(viewerDisplay->texture);
        SDL_DestroyRenderer(viewerDisplay->renderer);
        SDL_DestroyWindow(viewerDisplay->window);
    }
}

int SDL_ILBM_renderTexture(SDL_ILBM_ViewerDisplay *viewerDisplay)
{
    void *pixels;
    int pitch;

    if(SDL_LockTexture(viewerDisplay->texture, NULL, &pixels, &pitch) < 0)
    {
        fprintf(stderr, "Cannot lock texture: %s\n", SDL_GetError());
        return FALSE;
    }

    if(!SDL_ILBM_blitDisplayToTexture(&viewerDisplay->display, SDL_PIXELFORMAT_RGBA8888, pixels, pitch))
    {
        fprintf(stderr, "Cannot blit display to texture: %s\n", SDL_GetError());
        SDL_UnlockTexture(viewerDisplay->texture);
        return FALSE;
    }

    SDL_UnlockTexture(viewerDisplay->texture);

    if(SDL_ILBM_renderCopy(viewerDisplay->renderer, viewerDisplay->texture, viewerDisplay->offsetX, viewerDisplay->offsetY, &viewerDisplay->display) == 0)
        return TRUE;
    else
    {
        fprintf(stderr, "Cannot copy texture to the renderer: %s\n", SDL_GetError());
        return FALSE;
    }
}

int SDL_ILBM_scrollWindowLeft(SDL_ILBM_ViewerDisplay *viewerDisplay)
{
    if(viewerDisplay->offsetX > 0)
        viewerDisplay->offsetX--;

    return SDL_ILBM_renderTexture(viewerDisplay);
}

int SDL_ILBM_scrollWindowRight(SDL_ILBM_ViewerDisplay *viewerDisplay)
{
    if(viewerDisplay->offsetX < viewerDisplay->display.blitSurface->w - viewerDisplay->display.width)
        viewerDisplay->offsetX++;

    return SDL_ILBM_renderTexture(viewerDisplay);
}

int SDL_ILBM_scrollWindowUp(SDL_ILBM_ViewerDisplay *viewerDisplay)
{
    if(viewerDisplay->offsetY > 0)
        viewerDisplay->offsetY--;

    return SDL_ILBM_renderTexture(viewerDisplay);
}

int SDL_ILBM_scrollWindowDown(SDL_ILBM_ViewerDisplay *viewerDisplay)
{
    if(viewerDisplay->offsetY < viewerDisplay->display.blitSurface->h - viewerDisplay->display.height)
        viewerDisplay->offsetY++;

    return SDL_ILBM_renderTexture(viewerDisplay);
}
