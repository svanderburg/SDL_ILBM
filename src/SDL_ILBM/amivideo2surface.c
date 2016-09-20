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

#include "amivideo2surface.h"

int SDL_ILBM_setSurfacePaletteFromScreenPalette(amiVideo_Palette *palette, SDL_Surface *surface)
{
    return SDL_SetPaletteColors(surface->format->palette, (SDL_Color*)palette->chunkyFormat.color, 0, palette->chunkyFormat.numOfColors);
}

SDL_Surface *SDL_ILBM_createUncorrectedChunkySurfaceFromScreen(amiVideo_Screen *screen)
{
    SDL_Surface *surface = SDL_CreateRGBSurface(0, screen->width, screen->height, 8, 0, 0, 0, 0);
    
    /* Sets the uncorrected chunky pixels pointer of the conversion struct to that of the SDL pixel surface */
    amiVideo_setScreenUncorrectedChunkyPixelsPointer(screen, surface->pixels, surface->pitch);
    
    /* Convert the colors of the the bitplane palette to the format of the chunky palette */
    amiVideo_convertBitplaneColorsToChunkyFormat(&screen->palette);
    
    /* Set the palette of the target SDL surface */
    SDL_ILBM_setSurfacePaletteFromScreenPalette(&screen->palette, surface);
    
    return surface;
}

SDL_Surface *SDL_ILBM_createUncorrectedRGBSurfaceFromScreen(amiVideo_Screen *screen, const ILBM_Image *image)
{
    int allocateUncorrectedMemory = !ILBM_imageIsPBM(image);
    SDL_Surface *surface = SDL_CreateRGBSurface(0, screen->width, screen->height, 32, 0, 0, 0, 0);
    
    amiVideo_setScreenUncorrectedRGBPixelsPointer(screen, surface->pixels, surface->pitch, allocateUncorrectedMemory, surface->format->Rshift, surface->format->Gshift, surface->format->Bshift, surface->format->Ashift); /* Set the uncorrected RGB pixels pointer of the conversion struct to that of the SDL pixel surface */
    
    return surface;
}

SDL_Surface *SDL_ILBM_createCorrectedChunkySurfaceFromScreen(amiVideo_Screen *screen, const ILBM_Image *image, const unsigned int lowresPixelScaleFactor)
{
    SDL_Surface *surface;
    int allocateUncorrectedMemory = !ILBM_imageIsPBM(image);
    
    amiVideo_setLowresPixelScaleFactor(screen, lowresPixelScaleFactor);
    
    surface = SDL_CreateRGBSurface(0, screen->correctedFormat.width, screen->correctedFormat.height, 8, 0, 0, 0, 0);
    
    /* Set the corrected chunky pixels pointer of the conversion struct to the SDL pixel surface */
    amiVideo_setScreenCorrectedPixelsPointer(screen, surface->pixels, surface->pitch, 1, allocateUncorrectedMemory, 0, 0, 0, 0);
    
    /* Convert the colors of the the bitplane palette to the format of the chunky palette */
    amiVideo_convertBitplaneColorsToChunkyFormat(&screen->palette);
    
    /* Set the palette of the target SDL surface */
    SDL_ILBM_setSurfacePaletteFromScreenPalette(&screen->palette, surface);
    
    return surface;
}

SDL_Surface *SDL_ILBM_createCorrectedRGBSurfaceFromScreen(amiVideo_Screen *screen, const ILBM_Image *image, const unsigned int lowresPixelScaleFactor)
{
    SDL_Surface *surface;
    int allocateUncorrectedMemory = !ILBM_imageIsPBM(image);
    
    amiVideo_setLowresPixelScaleFactor(screen, lowresPixelScaleFactor);
    
    surface = SDL_CreateRGBSurface(0, screen->correctedFormat.width, screen->correctedFormat.height, 32, 0, 0, 0, 0);
    amiVideo_setScreenCorrectedPixelsPointer(screen, surface->pixels, surface->pitch, 4, allocateUncorrectedMemory, surface->format->Rshift, surface->format->Gshift, surface->format->Bshift, surface->format->Ashift); /* Set the corrected RGB pixels pointer of the conversion struct to the SDL pixel surface */
    
    return surface;
}
