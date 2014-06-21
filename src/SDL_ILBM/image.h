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

#include <SDL.h>
#include <libilbm/ilbmimage.h>
#include <libamivideo/screen.h>
#include <libamivideo/palette.h>

typedef enum
{
    SDL_ILBM_FORMAT_AUTO = 0,
    SDL_ILBM_FORMAT_CHUNKY = 1,
    SDL_ILBM_FORMAT_RGB = 4
}
SDL_ILBM_Format;

void SDL_ILBM_initPaletteFromImage(const ILBM_Image *image, amiVideo_Palette *palette);

amiVideo_ULong SDL_ILBM_extractViewportModeFromImage(const ILBM_Image *image);

void SDL_ILBM_initScreenFromImage(ILBM_Image *image, amiVideo_Screen *screen);

int SDL_ILBM_setPalette(SDL_Surface *surface, const amiVideo_Palette *palette);

int SDL_ILBM_convertScreenPixelsToSurfacePixels(const ILBM_Image *image, amiVideo_Screen *screen, SDL_Surface *surface, const SDL_ILBM_Format format, const unsigned int lowresPixelScaleFactor);

SDL_Surface *SDL_ILBM_generateSurfaceFromScreen(const ILBM_Image *image, amiVideo_Screen *screen, const SDL_ILBM_Format format, const unsigned int lowresPixelScaleFactor);

#endif
