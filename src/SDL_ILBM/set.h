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

#ifndef __SDL_ILBM_SET_H
#define __SDL_ILBM_SET_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_ILBM_Set SDL_ILBM_Set;

#include <stdio.h>
#include <SDL.h>
#include <libilbm/ilbmimage.h>
#include "image.h"

struct SDL_ILBM_Set
{
    IFF_Chunk *chunk;
    ILBM_Image **ilbmImages;
    unsigned int imagesLength;
    int mustFreeChunk;
};

int SDL_ILBM_initSetFromFilename(SDL_ILBM_Set *set, const char *filename);

int SDL_ILBM_initSetFromFd(SDL_ILBM_Set *set, FILE *file);

int SDL_ILBM_initSetFromIFFChunk(SDL_ILBM_Set *set, IFF_Chunk *chunk, int mustFreeChunk);

SDL_ILBM_Set *SDL_ILBM_createSetFromFilename(const char *filename);

SDL_ILBM_Set *SDL_ILBM_createSetFromFd(FILE *file);

SDL_ILBM_Set *SDL_ILBM_createSetFromIFFChunk(IFF_Chunk *chunk, int mustFreeChunk);

SDL_Surface *SDL_ILBM_createSurfaceFromSet(const SDL_ILBM_Set *set, const unsigned int index, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

int SDL_ILBM_initImageFromSet(const SDL_ILBM_Set *set, const unsigned int index, SDL_ILBM_Image *image, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

SDL_ILBM_Image *SDL_ILBM_createImageFromSet(const SDL_ILBM_Set *set, const unsigned int index, unsigned int lowresPixelScaleFactor, SDL_ILBM_Format format);

void SDL_ILBM_cleanupSet(SDL_ILBM_Set *set);

void SDL_ILBM_freeSet(SDL_ILBM_Set *set);

#ifdef __cplusplus
}
#endif

#endif
