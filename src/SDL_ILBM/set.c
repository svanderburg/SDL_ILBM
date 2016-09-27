#include "set.h"
#include <stdlib.h>
#include <libilbm/ilbm.h>

int SDL_ILBM_initSetFromFilename(SDL_ILBM_Set *set, const char *filename)
{
    FILE *file = fopen(filename, "rb");
    int status = SDL_ILBM_initSetFromFd(set, file);
    fclose(file);
    return status;
}

int SDL_ILBM_initSetFromFd(SDL_ILBM_Set *set, FILE *file)
{
    IFF_Chunk *chunk = ILBM_readFd(file);
    return SDL_ILBM_initSetFromIFFChunk(set, chunk, TRUE);
}

int SDL_ILBM_initSetFromIFFChunk(SDL_ILBM_Set *set, IFF_Chunk *chunk, int mustFreeChunk)
{
    set->chunk = chunk;
    set->mustFreeChunk = mustFreeChunk;
    set->ilbmImages = ILBM_extractImages(chunk, &set->imagesLength);
    
    return ILBM_checkImages(chunk, set->ilbmImages, set->imagesLength);
}

SDL_ILBM_Set *SDL_ILBM_createSetFromFilename(const char *filename)
{
    SDL_ILBM_Set *set = (SDL_ILBM_Set*)malloc(sizeof(SDL_ILBM_Set));
    
    if(set != NULL)
    {
        if(!SDL_ILBM_initSetFromFilename(set, filename))
        {
            SDL_ILBM_freeSet(set);
            return NULL;
        }
    }
    
    return set;
}

SDL_ILBM_Set *SDL_ILBM_createSetFromFd(FILE *file)
{
    SDL_ILBM_Set *set = (SDL_ILBM_Set*)malloc(sizeof(SDL_ILBM_Set));
    
    if(set != NULL)
    {
        if(!SDL_ILBM_initSetFromFd(set, file))
        {
            SDL_ILBM_freeSet(set);
            return NULL;
        }
    }
    
    return set;
}

SDL_ILBM_Set *SDL_ILBM_createSetFromIFFChunk(IFF_Chunk *chunk, int mustFreeChunk)
{
    SDL_ILBM_Set *set = (SDL_ILBM_Set*)malloc(sizeof(SDL_ILBM_Set));
    
    if(set != NULL)
    {
        if(!SDL_ILBM_initSetFromIFFChunk(set, chunk, mustFreeChunk))
        {
            SDL_ILBM_freeSet(set);
            return NULL;
        }
    }
    
    return set;
}

SDL_Surface *SDL_ILBM_createSurfaceFromSet(const SDL_ILBM_Set *set, const unsigned int index, unsigned int lowresPixelScaleFactor, SDL_ILBM_Format format)
{
    if(index < set->imagesLength)
        return SDL_ILBM_createSurface(set->ilbmImages[index], lowresPixelScaleFactor, format);
    else
        return NULL;
}

int SDL_ILBM_initImageFromSet(const SDL_ILBM_Set *set, const unsigned int index, SDL_ILBM_Image *image, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format)
{
    if(index < set->imagesLength)
        return SDL_ILBM_initImage(image, set->ilbmImages[index], lowresPixelScaleFactor, format);
    else
        return FALSE;
}

SDL_ILBM_Image *SDL_ILBM_createImageFromSet(const SDL_ILBM_Set *set, const unsigned int index, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format)
{
    if(index < set->imagesLength)
        return SDL_ILBM_createImage(set->ilbmImages[index], lowresPixelScaleFactor, format);
    else
        return NULL;
}

void SDL_ILBM_cleanupSet(SDL_ILBM_Set *set)
{
    ILBM_freeImages(set->ilbmImages, set->imagesLength);
    
    if(set->mustFreeChunk)
        ILBM_free(set->chunk);
}

void SDL_ILBM_freeSet(SDL_ILBM_Set *set)
{
    if(set != NULL)
    {
        SDL_ILBM_cleanupSet(set);
        free(set);
    }
}
