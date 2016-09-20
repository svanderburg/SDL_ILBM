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

#include "render.h"
#include <string.h>

int SDL_ILBM_renderUncorrectedChunkyImage(const ILBM_Image *image, amiVideo_Screen *screen, SDL_Surface *surface)
{
    if(SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) != 0)
    {
        fprintf(stderr, "Cannot lock the surface!\n");
        return FALSE;
    }
    
    if(ILBM_imageIsPBM(image))
    {
        if(image->body != NULL)
            memcpy(screen->uncorrectedChunkyFormat.pixels, image->body->chunkData, image->body->chunkSize); /* For a PBM no conversion is needed => simply copy the data into the surface */
    }
    else
    {
        if(image->bitplanes != NULL)
            amiVideo_convertScreenBitplanesToChunkyPixels(screen); /* Convert the bitplanes to chunky pixels */
    }
    
    if(SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
    
    return TRUE;
}

int SDL_ILBM_renderUncorrectedRGBImage(const ILBM_Image *image, amiVideo_Screen *screen, SDL_Surface *surface)
{
    if(SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) != 0)
    {
        fprintf(stderr, "Cannot lock the surface!\n");
        return FALSE;
    }
    
    if(ILBM_imageIsPBM(image))
    {
        if(image->body != NULL)
        {
            if(screen->bitplaneDepth == 24 || screen->bitplaneDepth == 32)
            {
                /* For images with a higher than 8 bitplane depth (true color images) => copy chunky data to the RGB section and reorder the pixels */
                memcpy(screen->uncorrectedRGBFormat.pixels, image->body->chunkData, image->body->chunkSize);
                amiVideo_reorderRGBPixels(screen);
            }
            else
                amiVideo_convertScreenChunkyPixelsToRGBPixels(screen); /* Convert chunky to RGB data */
        }
    }
    else
    {
        if(image->bitplanes != NULL)
            amiVideo_convertScreenBitplanesToRGBPixels(screen); /* Convert the bitplanes to RGB pixels */
    }
    
    if(SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
    
    return TRUE;
}

int SDL_ILBM_renderCorrectedChunkyImage(const ILBM_Image *image, amiVideo_Screen *screen, SDL_Surface *surface)
{
    if(SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) != 0)
    {
        fprintf(stderr, "Cannot lock the surface!\n");
        return FALSE;
    }
    
    if(ILBM_imageIsPBM(image))
    {
        if(image->body != NULL)
            amiVideo_correctScreenPixels(screen); /* Correct the pixels */
    }
    else
    {
        if(image->bitplanes != NULL)
            amiVideo_convertScreenBitplanesToCorrectedChunkyPixels(screen); /* Convert the bitplanes to corrected chunky pixels */
    }
    
    if(SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
    
    return TRUE;
}

int SDL_ILBM_renderCorrectedRGBImage(const ILBM_Image *image, amiVideo_Screen *screen, SDL_Surface *surface)
{
    if(SDL_MUSTLOCK(surface) && SDL_LockSurface(surface) != 0)
    {
        fprintf(stderr, "Cannot lock the surface!\n");
        return FALSE;
    }
    
    if(ILBM_imageIsPBM(image))
    {
        if(image->body != NULL)
        {
            if(screen->bitplaneDepth == 24 || screen->bitplaneDepth == 32)
            {
                /* For images with a higher than 8 bitplane depth => copy chunky data to the RGB section, reorder the pixels, and correct the image */
                memcpy(screen->uncorrectedRGBFormat.pixels, image->body->chunkData, image->body->chunkSize);
                amiVideo_reorderRGBPixels(screen);
                amiVideo_correctScreenPixels(screen);
            }
            else
                amiVideo_convertScreenChunkyPixelsToCorrectedRGBPixels(screen); /* Convert bitplanes to RGB pixels and correct the image */
        }
    }
    else
    {
        if(image->bitplanes != NULL)
            amiVideo_convertScreenBitplanesToCorrectedRGBPixels(screen); /* Convert the bitplanes to corrected RGB pixels */
    }
    
    if(SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
    
    return TRUE;
}
