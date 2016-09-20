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

#include "image.h"
#include <stdlib.h>
#include <libamivideo/viewportmode.h>
#include "image2amivideo.h"
#include "amivideo2surface.h"
#include "render.h"
#include "cycle.h"

/* Choose appropriate lowres pixel scale factor */

static int selectLowresPixelScaleFactor(const unsigned int lowresPixelScaleFactor, amiVideo_Long viewportMode)
{
    if(lowresPixelScaleFactor > 0)
        return lowresPixelScaleFactor;
    else
        return amiVideo_autoSelectLowresPixelScaleFactor(viewportMode);
}

/* Choose appropriate color format */

static amiVideo_ColorFormat selectColorFormat(const SDL_ILBM_Format format, const amiVideo_Screen *screen)
{
    if(format == SDL_ILBM_AUTO_FORMAT)
        return amiVideo_autoSelectColorFormat(screen);
    else
        return format;
}

static SDL_Surface *createSurfaceFromScreen(amiVideo_Screen *screen, ILBM_Image *image, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format)
{
    unsigned int realLowresPixelScaleFactor;
    SDL_ILBM_Format realFormat;
    SDL_Surface *surface;
    
    /* Attach the image to screen conversion pipeline */
    SDL_ILBM_attachImageToScreen(image, screen);
    
    /* Calculate real values */
    realLowresPixelScaleFactor = selectLowresPixelScaleFactor(lowresPixelScaleFactor, screen->viewportMode);
    realFormat = selectColorFormat(format, screen);
    
    /* Create and render the surface */
    if(realLowresPixelScaleFactor > 1)
    {
        if(realFormat == SDL_ILBM_CHUNKY_FORMAT)
        {
            surface = SDL_ILBM_createCorrectedChunkySurfaceFromScreen(screen, image, realLowresPixelScaleFactor);
            SDL_ILBM_renderCorrectedChunkyImage(image, screen, surface);
        }
        else
        {
            surface = SDL_ILBM_createCorrectedRGBSurfaceFromScreen(screen, image, realLowresPixelScaleFactor);
            SDL_ILBM_renderCorrectedRGBImage(image, screen, surface);
        }
    }
    else
    {
        if(realFormat == SDL_ILBM_CHUNKY_FORMAT)
        {
            surface = SDL_ILBM_createUncorrectedChunkySurfaceFromScreen(screen);
            SDL_ILBM_renderUncorrectedChunkyImage(image, screen, surface);
        }
        else
        {
            surface = SDL_ILBM_createUncorrectedRGBSurfaceFromScreen(screen, image);
            SDL_ILBM_renderUncorrectedRGBImage(image, screen, surface);
        }
    }
    
    return surface;
}

SDL_Surface *SDL_ILBM_createSurface(ILBM_Image *image, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format)
{
    amiVideo_Screen screen;
    return createSurfaceFromScreen(&screen, image, lowresPixelScaleFactor, format);
}

static int updateChunkyPalette(const ILBM_Image *image, amiVideo_Screen *screen, SDL_Surface *surface)
{
    amiVideo_convertBitplaneColorsToChunkyFormat(&screen->palette);
    return SDL_ILBM_setSurfacePaletteFromScreenPalette(&screen->palette, surface);
}

int SDL_ILBM_initImage(SDL_ILBM_Image *image, ILBM_Image *ilbmImage, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format)
{
    /* Attach some properties to the facade */
    image->image = ilbmImage;
    
    /* Initialise the range times */
    SDL_ILBM_initRangeTimes(&image->rangeTimes, image->image);
    
    /* Create and initially render the surface */
    image->surface = createSurfaceFromScreen(&image->screen, image->image, lowresPixelScaleFactor, format);
    
    /* Memorize real values. TODO: duplicate, maybe somewhere else? */
    image->format = selectColorFormat(format, &image->screen);
    image->lowresPixelScaleFactor = selectLowresPixelScaleFactor(lowresPixelScaleFactor, image->screen.viewportMode);
    
    /* Pick palette update function */
    if(image->format == SDL_ILBM_CHUNKY_FORMAT)
        image->updatePaletteAndSurface = updateChunkyPalette; /* For chunky/8-bit surfaces, we simply need to modify its palette and then reblit it */
    else
    {
        /* In addition to palette changes, RGB surfaces needs to be redrawn entirely */
        if(image->lowresPixelScaleFactor > 1)
            image->updatePaletteAndSurface = SDL_ILBM_renderCorrectedRGBImage;
        else
            image->updatePaletteAndSurface = SDL_ILBM_renderUncorrectedRGBImage;
    }
    
    return (image->surface != NULL);
}

SDL_ILBM_Image *SDL_ILBM_createImage(ILBM_Image *ilbmImage, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format)
{
    SDL_ILBM_Image *image = (SDL_ILBM_Image*)malloc(sizeof(SDL_ILBM_Image));
    
    if(image != NULL)
    {
        if(!SDL_ILBM_initImage(image, ilbmImage, lowresPixelScaleFactor, format))
        {
            SDL_ILBM_freeImage(image);
            return NULL;
        }
    }
    
    return image;
}

int SDL_ILBM_blitImageToSurface(SDL_ILBM_Image *image, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
    return SDL_BlitSurface(image->surface, srcrect, dst, dstrect);
}

void SDL_ILBM_destroyImage(SDL_ILBM_Image *image)
{
    SDL_FreeSurface(image->surface);
    SDL_ILBM_cleanupRangeTimes(&image->rangeTimes);
}

void SDL_ILBM_freeImage(SDL_ILBM_Image *image)
{
    if(image != NULL)
    {
        SDL_ILBM_destroyImage(image);
        free(image);
    }
}

void SDL_ILBM_cycleColors(SDL_ILBM_Image *image)
{
    SDL_ILBM_shiftActiveRanges(&image->rangeTimes, image->image, &image->screen.palette);
    image->updatePaletteAndSurface(image->image, &image->screen, image->surface);
}

void SDL_ILBM_resetColors(SDL_ILBM_Image *image)
{
    SDL_ILBM_initPaletteFromImage(image->image, &image->screen.palette);
    image->updatePaletteAndSurface(image->image, &image->screen, image->surface);
}
