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

/**
 * @brief An encapsulation of a set of images that originate from an IFF/ILBM file.
 */
struct SDL_ILBM_Set
{
    /** Reference to a parsed chunk originating from an IFF file */
    IFF_Chunk *chunk;

    /** An array extracted ILBM images from an IFF file */
    ILBM_Image **ilbmImages;

    /** Specifies the length of the ILBM images array */
    unsigned int imagesLength;

    /* Indicates whether the chunk must be deallocated while freeing the set */
    IFF_Bool mustFreeChunk;
};

/**
 * Initializes a preallocated set by opening a file with a specific file
 * descriptor.
 *
 * @param set A preallocated set
 * @param file File descriptor
 * @return TRUE if the initialization succeeded, else FALSE
 */
IFF_Bool SDL_ILBM_initSetFromFd(SDL_ILBM_Set *set, FILE *file);

/**
 * Initializes a preallocated set by opening a file with a specified filename.
 *
 * @param set A preallocated set
 * @param filename Path to an IFF file to open
 * @return TRUE if the initialization succeeded, else FALSE
 */
IFF_Bool SDL_ILBM_initSetFromFilename(SDL_ILBM_Set *set, const char *filename);

/**
 * Initializes a preallocated set by opening a file with a specified filename
 * or reading it from the standard input if no filename was provided.
 *
 * @param set A preallocated set
 * @param filename Path to an IFF file to open or NULL to read from the standard input
 * @return TRUE if the initialization succeeded, else FALSE
 */
IFF_Bool SDL_ILBM_initSet(SDL_ILBM_Set *set, const char *filename);

/**
 * Initializes a preallocated set by parsing an IFF chunk.
 *
 * @param set A preallocated set
 * @param chunk IFF chunk originating from a parsed file
 * @param mustFreeChunk Indicates whether the provided chunk must be freed from memory while freeing the set
 * @return TRUE if the set was successfully generated, else FALSE
 */
IFF_Bool SDL_ILBM_initSetFromIFFChunk(SDL_ILBM_Set *set, IFF_Chunk *chunk, IFF_Bool mustFreeChunk);

/**
 * Creates a set by opening a file with a specific file descriptor.
 *
 * @param file File descriptor
 * @return An SDL_ILBM_Set instance or NULL in case of an error. The resulting set must be freed with SDL_ILBM_freeSet()
 */
SDL_ILBM_Set *SDL_ILBM_createSetFromFd(FILE *file);

/**
 * Creates a set by opening a file with a specified filename.
 *
 * @param filename Path to an IFF file to open
 * @return An SDL_ILBM_Set instance or NULL in case of an error. The resulting set must be freed with SDL_ILBM_freeSet()
 */
SDL_ILBM_Set *SDL_ILBM_createSetFromFilename(const char *filename);

/**
 * Creates a set by opening a file with a specified filename or reading it from the standard input when no filename was provided.
 *
 * @param filename Path to an IFF file to open or NULL to read from the standard input
 * @return An SDL_ILBM_Set instance or NULL in case of an error. The resulting set must be freed with SDL_ILBM_freeSet()
 */
SDL_ILBM_Set *SDL_ILBM_createSet(const char *filename);

/**
 * Creates a set by parsing an IFF chunk.
 *
 * @param chunk IFF chunk originating from a parsed file
 * @param mustFreeChunk Indicates whether the provided chunk must be freed from memory while freeing the set
 * @return An SDL_ILBM_Set instance or NULL in case of an error. The resulting set must be freed with SDL_ILBM_freeSet()
 */
SDL_ILBM_Set *SDL_ILBM_createSetFromIFFChunk(IFF_Chunk *chunk, IFF_Bool mustFreeChunk);

/**
 * Creates an SDL_Surface from an image in the set.
 *
 * @param set An SDL_ILBM_Set containing images
 * @param index Index of the image in the set
 * @param lowresPixelScaleFactor Specifies the width of a lowres pixel
 * @param format Defines to which format the output must be converted
 * @return An SDL_Surface or NULL in case of an error. The result surface must be freed with SDL_FreeSurface()
 */
SDL_Surface *SDL_ILBM_createSurfaceFromSet(const SDL_ILBM_Set *set, const unsigned int index, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

/**
 * Initializes a preallocated cyclable SDL_ILBM_Image from an image in the set.
 *
 * @param set An SDL_ILBM_Set containing images
 * @param index Index of the image in the set
 * @param lowresPixelScaleFactor Specifies the width of a lowres pixel
 * @param format Defines to which format the output must be converted
 * @return TRUE if the initialization succeeded, else FALSE
 */
IFF_Bool SDL_ILBM_initImageFromSet(const SDL_ILBM_Set *set, const unsigned int index, SDL_ILBM_Image *image, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

/**
 * Creates a cyclable SDL_ILBM_Image from an image in the set.
 *
 * @param set An SDL_ILBM_Set containing images
 * @param index Index of the image in the set
 * @param lowresPixelScaleFactor Specifies the width of a lowres pixel
 * @param format Defines to which format the output must be converted
 * @return An SDL_ILBM_Image or NULL in case of an error. The resulting image must be freed with SDL_ILBM_freeImage()
 */
SDL_ILBM_Image *SDL_ILBM_createImageFromSet(const SDL_ILBM_Set *set, const unsigned int index, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format);

/**
 * Clears all properties of a set from memory.
 *
 * @param set An SDL_ILBM_Set containing images
 */
void SDL_ILBM_cleanupSet(SDL_ILBM_Set *set);

/**
 * Frees a set from memory.
 *
 * @param set An SDL_ILBM_Set containing images
 */
void SDL_ILBM_freeSet(SDL_ILBM_Set *set);

#ifdef __cplusplus
}
#endif

#endif
