/**
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
#include <libilbm/interleave.h>
#include <libilbm/byterun.h>
#include <libamivideo/palette.h>
#include <libamivideo/bitplane.h>
#include "colors.h"

SDL_Surface *SDL_ILBM_createChunkySurface(ILBM_Image *image, SDL_Color *colors, const unsigned int colorsLength)
{
    IFF_UByte *indexes;
    SDL_Surface *picture;
    AMI_UByte *indexPixels;
    
    /* Decompress the image body */
    ILBM_unpackByteRun(image);
    
    indexes = ILBM_deinterleave(image);
    
    picture = SDL_CreateRGBSurface(SDL_SWSURFACE, image->bitMapHeader->w, image->bitMapHeader->h, 8, 0, 0, 0, 0);
    
    if(picture == NULL)
    {
	fprintf(stderr, "Cannot create picture surface!\n");
	return NULL;
    }
    
    indexPixels = amiVideo_bitplanesToChunky(indexes, image->bitMapHeader->w, image->bitMapHeader->h, image->bitMapHeader->nPlanes);
    
    /* Attach pixels */
    
    if(SDL_LockSurface(picture) < 0)
    {
	fprintf(stderr, "Error locking surface: %s\n", SDL_GetError());
	return NULL;
    }
    
    picture->pixels = indexPixels;
    
    SDL_UnlockSurface(picture);
    
    SDL_SetPalette(picture, SDL_LOGPAL | SDL_PHYSPAL, colors, 0, colorsLength);
    
    free(indexes);
    
    return picture;
}

void SDL_ILBM_updatePalette(SDL_Color *colors, unsigned int colorsLength, SDL_Surface *picture, SDL_Surface *screen, int xOffset, int yOffset)
{
    SDL_SetPalette(picture, SDL_LOGPAL | SDL_PHYSPAL, (SDL_Color*)colors, 0, colorsLength);
    SDL_ILBM_drawPicture(picture, screen, xOffset, yOffset);
}

void SDL_ILBM_drawPicture(SDL_Surface *src, SDL_Surface *dest, int xOffset, int yOffset)
{
    SDL_Rect dstrect;
    dstrect.x = -xOffset;
    dstrect.y = -yOffset;
    
    if(SDL_BlitSurface(src, NULL, dest, &dstrect) < 0)
	fprintf(stderr, "Error blitting surface: %s!\n", SDL_GetError());
}

SDL_Surface *SDL_ILBM_createRGBASurface(ILBM_Image *image)
{
    IFF_UByte *indexes;
    SDL_Surface *picture;
    AMI_OutputColor *colorPixels;
    Uint32 *rgbPixels;
    unsigned int i;
    
    /* Decompress the image body */
    ILBM_unpackByteRun(image);
    
    indexes = ILBM_deinterleave(image);
    
    picture = SDL_CreateRGBSurface(SDL_SWSURFACE, image->bitMapHeader->w, image->bitMapHeader->h, 32, 0, 0, 0, 0);
    
    if(picture == NULL)
    {
	fprintf(stderr, "Cannot create picture surface!\n");
	return NULL;
    }
    
    colorPixels = amiVideo_bitplanesToRGB(indexes, image->bitMapHeader->w, image->bitMapHeader->h, image->bitMapHeader->nPlanes, (AMI_Color*)image->colorMap->colorRegister, image->colorMap->colorRegisterLength, image->viewport->viewportMode);
    
    rgbPixels = (Uint32*)malloc(image->bitMapHeader->w * image->bitMapHeader->h * sizeof(Uint32));
	
    for(i = 0; i < image->bitMapHeader->w * image->bitMapHeader->h; i++)
        rgbPixels[i] = SDL_MapRGB(picture->format, colorPixels[i].r, colorPixels[i].g, colorPixels[i].b);
    
    /* Attach pixels */
    
    if(SDL_LockSurface(picture) < 0)
    {
	fprintf(stderr, "Error locking surface: %s\n", SDL_GetError());
	return NULL;
    }
    
    picture->pixels = rgbPixels;
    
    SDL_UnlockSurface(picture);
    
    free(colorPixels);
    free(indexes);
    
    return picture;
}
