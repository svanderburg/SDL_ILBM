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

#ifndef __SDL_ILBM_IMAGE_H
#define __SDL_ILBM_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_ILBM_Image SDL_ILBM_Image;

#include <SDL.h>
#include <libilbm/ilbmimage.h>
#include <libamivideo/screen.h>
#include "cycle.h"

typedef enum
{
    SDL_ILBM_AUTO_FORMAT = 0,
    SDL_ILBM_CHUNKY_FORMAT = 1,
    SDL_ILBM_RGB_FORMAT = 4
}
SDL_ILBM_Format;

struct SDL_ILBM_Image
{
    ILBM_Image *image;
    amiVideo_Screen screen;
    SDL_Surface *surface;
    SDL_ILBM_RangeTimes rangeTimes;
    int lowresPixelScaleFactor;
    SDL_ILBM_Format format;
    int (*updatePaletteAndSurface) (const ILBM_Image *image, amiVideo_Screen *screen, SDL_Surface *surface);
};

SDL_Surface *SDL_ILBM_createSurface(ILBM_Image *image, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

int SDL_ILBM_initImage(SDL_ILBM_Image *image, ILBM_Image *ilbmImage, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

SDL_ILBM_Image *SDL_ILBM_createImage(ILBM_Image *ilbmImage, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

int SDL_ILBM_blitImageToSurface(SDL_ILBM_Image *image, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);

void SDL_ILBM_destroyImage(SDL_ILBM_Image *image);

void SDL_ILBM_freeImage(SDL_ILBM_Image *image);

void SDL_ILBM_cycleColors(SDL_ILBM_Image *image);

void SDL_ILBM_resetColors(SDL_ILBM_Image *image);

#ifdef __cplusplus
}
#endif

#endif
