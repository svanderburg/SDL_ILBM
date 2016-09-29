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

/**
 * @brief Enumerates all possible output formats this API supports.
 */
typedef enum
{
    SDL_ILBM_AUTO_FORMAT = 0,
    SDL_ILBM_CHUNKY_FORMAT = 1,
    SDL_ILBM_RGB_FORMAT = 4
}
SDL_ILBM_Format;

/**
 * @brief Encapsulates the properties of a cyclable image
 */
struct SDL_ILBM_Image
{
    /** Reference to the ILBM image from which this image is generated */
    ILBM_Image *image;
    
    /** Reference to the screen converting the image to the desired output format */
    amiVideo_Screen screen;
    
    /** Reference to the SDL Surface that can be blitted */
    SDL_Surface *surface;
    
    /** Maintains the ellapsed cycle range times */
    SDL_ILBM_RangeTimes rangeTimes;
    
    /** Specifies the width of a lowres pixel */
    int lowresPixelScaleFactor;
    
    /** Defines to which format the output must be converted */
    SDL_ILBM_Format format;
    
    /** Function that must be executed to update the palette and surface each time a color cycles. This function is for internal use only. */
    int (*updatePaletteAndSurface) (const ILBM_Image *image, amiVideo_Screen *screen, SDL_Surface *surface);
};

/**
 * Composes an SDL Surface from a given ILBM image in a specified output format.
 *
 * @param image ILBM image to generate the output from
 * @param lowresPixelScaleFactor Specifies the width of a lowres pixel
 * @param format Defines to which format the output must be converted
 * @return An SDL surface that can be blitted to another surface. The surface must be freed with SDL_FreeSurface()
 */
SDL_Surface *SDL_ILBM_createSurface(ILBM_Image *image, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

/**
 * Initializes a preallocated SDL_ILBM_Image from a given ILBM image in a specified
 * output format.
 *
 * @param image Preallocated SDL_ILBM_Image instance
 * @param ilbmImage ILBM image to generate the output from
 * @param lowresPixelScaleFactor Specifies the width of a lowres pixel
 * @param format Defines to which format the output must be converted
 * @return TRUE if the initalisation succeeded, otherwise FALSE
 */
int SDL_ILBM_initImage(SDL_ILBM_Image *image, ILBM_Image *ilbmImage, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

/**
 * Composes an SDL_ILBM_Image from a given ILBM image in a specified output format.
 *
 * @param ilbmImage ILBM image to generate the output from
 * @param lowresPixelScaleFactor Specifies the width of a lowres pixel
 * @param format Defines to which format the output must be converted
 * @return An SDL_ILBM_Image instance or NULL in case of an error. The result must be freed with SDL_ILBM_freeImage()
 */
SDL_ILBM_Image *SDL_ILBM_createImage(ILBM_Image *ilbmImage, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

/**
 * Blits an SDL_ILBM_Image to a provided SDL Surface, optionally restricting the
 * source and destination areas to a specific subsets. This function is
 * basically a wrapper around SDL's SDL_BlitSurface() function.
 *
 * @param image An SDL_ILBM_Image instance
 * @param srcrect Rectangle restricting the area of the source surface or NULL to pick the entire area
 * @param dst Destination SDL Surface
 * @param dstrect Rectangle restricting the area of the destination surface or NULL to pick the entire area
 * @returns 0 in case of a success or a value below zero in case of a failure
 */
int SDL_ILBM_blitImageToSurface(SDL_ILBM_Image *image, const SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);

/**
 * Clears the properties of the provided image from memory.
 * 
 * @param image An SDL_ILBM_Image instance
 */
void SDL_ILBM_destroyImage(SDL_ILBM_Image *image);

/**
 * Frees the provided image from memory.
 * 
 * @param image An SDL_ILBM_Image instance
 */
void SDL_ILBM_freeImage(SDL_ILBM_Image *image);

/**
 * Checks whether the range times have ellapsed and cycles the
 * corresponding colors in the palette accordingly.
 *
 * @param image An SDL_ILBM_Image instance
 */
void SDL_ILBM_cycleColors(SDL_ILBM_Image *image);

/**
 * Resets the colors in the palette back to normal.
 *
 * @param image An SDL_ILBM_Image instance
 */
void SDL_ILBM_resetColors(SDL_ILBM_Image *image);

#ifdef __cplusplus
}
#endif

#endif
