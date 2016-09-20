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

#ifndef __SDL_ILBM_AMIVIDEO2SURFACE_H
#define __SDL_ILBM_AMIVIDEO2SURFACE_H
#include <SDL.h>
#include <libilbm/ilbmimage.h>
#include <libamivideo/palette.h>
#include <libamivideo/screen.h>

#ifdef __cplusplus
extern "C" {
#endif

int SDL_ILBM_setSurfacePaletteFromScreenPalette(amiVideo_Palette *palette, SDL_Surface *surface);

SDL_Surface *SDL_ILBM_createUncorrectedChunkySurfaceFromScreen(amiVideo_Screen *screen);

SDL_Surface *SDL_ILBM_createUncorrectedRGBSurfaceFromScreen(amiVideo_Screen *screen, const ILBM_Image *image);

SDL_Surface *SDL_ILBM_createCorrectedChunkySurfaceFromScreen(amiVideo_Screen *screen, const ILBM_Image *image, const unsigned int lowresPixelScaleFactor);

SDL_Surface *SDL_ILBM_createCorrectedRGBSurfaceFromScreen(amiVideo_Screen *screen, const ILBM_Image *image, const unsigned int lowresPixelScaleFactor);

#ifdef __cplusplus
}
#endif

#endif
