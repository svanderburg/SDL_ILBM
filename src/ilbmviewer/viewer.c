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

#include "viewer.h"
#include <stdio.h>
#include <SDL.h>
#include <libilbm/ilbm.h>
#include <libilbm/ilbmimage.h>
#include <libamivideo/screen.h>
#include <libamivideo/viewportmode.h>
#include "window.h"
#include "image.h"
#include "cycle.h"

typedef enum
{
    SDL_ILBM_STATUS_NONE,
    SDL_ILBM_STATUS_QUIT,
    SDL_ILBM_STATUS_PREVIOUS,
    SDL_ILBM_STATUS_NEXT,
    SDL_ILBM_STATUS_ERROR
}
SDL_ILBM_Status;

static int updatePaletteAndSurface(amiVideo_Screen *screen, const ILBM_Image *image, const SDL_ILBM_Format format, const unsigned int lowresPixelScaleFactor, SDL_Surface *pictureSurface, SDL_ILBM_Window *window)
{
    if(format == SDL_ILBM_FORMAT_CHUNKY)
    {
        /* Switch palette colors for chunky graphics which can be done quite efficiently */
        amiVideo_convertBitplaneColorsToChunkyFormat(&screen->palette);
        SDL_ILBM_setPalette(pictureSurface, &screen->palette);
    }
    else
        SDL_ILBM_convertScreenPixelsToSurfacePixels(image, screen, pictureSurface, format, lowresPixelScaleFactor); /* Do the complete conversion cycle which is more work */
    
    /* Reblit the picture surface on the window */
    return SDL_ILBM_blitPictureInWindow(window);
}

static SDL_ILBM_Status viewILBMImage(ILBM_Image *image, const SDL_ILBM_Format format, const unsigned int number, const unsigned int imagesLength, const unsigned int lowresPixelScaleFactor, const unsigned int options)
{
    SDL_ILBM_Window window;
    SDL_Surface *pictureSurface;
    SDL_ILBM_RangeTimes rangeTimes;
    amiVideo_Screen screen;
    unsigned int realLowresPixelScaleFactor;
    SDL_ILBM_Format realFormat;
    SDL_ILBM_Status status = SDL_ILBM_STATUS_NONE;
    int cycle, fullscreen, stretch;
    
    /* Initialize conversion screen */
    SDL_ILBM_initScreenFromImage(image, &screen);
    
    /* Auto select lowres pixel scale factor, if needed */
    if(lowresPixelScaleFactor == 0)
        realLowresPixelScaleFactor = amiVideo_autoSelectLowresPixelScaleFactor(screen.viewportMode);
    else
        realLowresPixelScaleFactor = lowresPixelScaleFactor;

    /* Auto select format, if needed */
    if(format == SDL_ILBM_FORMAT_AUTO)
        realFormat = amiVideo_autoSelectColorFormat(&screen);
    else
        realFormat = format;

    /* Determine fullscreen option */
    if(options & SDL_ILBM_OPTION_FULLSCREEN)
        fullscreen = TRUE;
    else
        fullscreen = FALSE;
    
    /* Determine whether to stretch */
    if(options & SDL_ILBM_OPTION_STRETCH)
        stretch = TRUE;
    else
        stretch = FALSE;

    /* Determine whether to cycle */
    if(options & SDL_ILBM_OPTION_CYCLE)
        cycle = TRUE;
    else
        cycle = FALSE;
    
    /* Convert the image */
    pictureSurface = SDL_ILBM_generateSurfaceFromScreen(image, &screen, realFormat, realLowresPixelScaleFactor);
    
    if(pictureSurface == NULL)
    {
        fprintf(stderr, "Cannot generate surface from screen!\n");
        amiVideo_cleanupScreen(&screen);
        return SDL_ILBM_STATUS_QUIT;
    }
    
    /* Initialize window */
    SDL_ILBM_initWindow(&window, "SDL ILBM Viewer", image, &screen, pictureSurface, fullscreen, stretch, realLowresPixelScaleFactor);
    
    /* Initialize cycle times */
    SDL_ILBM_initRangeTimes(&rangeTimes, image);
    
    /* Blit the picture in the window */
    if(!SDL_ILBM_blitPictureInWindow(&window))
        status = SDL_ILBM_STATUS_QUIT;

    /* Main loop taking care of user events */
    while(status == SDL_ILBM_STATUS_NONE)
    {
        SDL_Event event;
        
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym)
                    {
                        case SDLK_f:
                                if(!SDL_ILBM_toggleWindowFullscreen(&window))
                                    status = SDL_ILBM_STATUS_QUIT;
                            break;
                        
                        case SDLK_s:
                                if(!SDL_ILBM_toggleWindowStretch(&window))
                                    status = SDL_ILBM_STATUS_QUIT;
                            break;
                        
                        case SDLK_ESCAPE:
                            status = SDL_ILBM_STATUS_QUIT;
                            break;
                        
                        case SDLK_SPACE:
                        case SDLK_PAGEDOWN:
                            if(number < imagesLength - 1)
                                status = SDL_ILBM_STATUS_NEXT;
                            break;
                            
                        case SDLK_PAGEUP:
                            if(number > 0)
                                status = SDL_ILBM_STATUS_PREVIOUS;
                            break;
                        
                        case SDLK_TAB:
                            cycle = !cycle;
                            
                            if(!cycle)
                            {
                                SDL_ILBM_initPaletteFromImage(image, &screen.palette);
                                if(!updatePaletteAndSurface(&screen, image, realFormat, realLowresPixelScaleFactor, pictureSurface, &window))
                                    status = SDL_ILBM_STATUS_QUIT;
                            }
                            break;
                        
                        case SDLK_LEFT:
                            if(!SDL_ILBM_scrollWindowLeft(&window))
                                status = SDL_ILBM_STATUS_QUIT;
                            break;
                        
                        case SDLK_RIGHT:
                            if(!SDL_ILBM_scrollWindowRight(&window))
                                status = SDL_ILBM_STATUS_QUIT;
                            break;
                        
                        case SDLK_UP:
                            if(!SDL_ILBM_scrollWindowUp(&window))
                                status = SDL_ILBM_STATUS_QUIT;
                            break;
                        
                        case SDLK_DOWN:
                            if(!SDL_ILBM_scrollWindowDown(&window))
                                status = SDL_ILBM_STATUS_QUIT;
                            break;
                    }
                    break;
                    
                case SDL_QUIT:
                    status = SDL_ILBM_STATUS_QUIT;
                    break;
            }
        }
        
        /* If cycle mode is enabled, do the work that is needed to switch the colors */
        if(cycle)
        {
            SDL_ILBM_shiftActiveRanges(&rangeTimes, image, &screen.palette);
            if(!updatePaletteAndSurface(&screen, image, realFormat, realLowresPixelScaleFactor, pictureSurface, &window))
                status = SDL_ILBM_STATUS_QUIT;
        }
        
        /* Flip screen buffers, so that changes become visible */
        SDL_RenderPresent(window.renderer);
    }
    
    /* Cleanup */
    SDL_ILBM_cleanupRangeTimes(&rangeTimes);
    SDL_FreeSurface(pictureSurface);
    amiVideo_cleanupScreen(&screen);
    SDL_ILBM_destroyWindow(&window);
    
    /* Return exit status */
    return status;
}

int SDL_ILBM_viewILBMImages(const char *filename, const SDL_ILBM_Format format, unsigned int number, const unsigned int lowresPixelScaleFactor, const unsigned int options)
{
    IFF_Chunk *chunk;
    ILBM_Image **images;
    unsigned int imagesLength;
    SDL_ILBM_Status status = SDL_ILBM_STATUS_NONE;
    
    /* Open the ILBM file */
    
    if(filename == NULL)
        chunk = ILBM_readFd(stdin);
    else
        chunk = ILBM_read(filename);
    
    if(chunk == NULL)
    {
        fprintf(stderr, "Error parsing ILBM file!\n");
        return 1;
    }
    
    /* Extract images */
    images = ILBM_extractImages(chunk, &imagesLength);

    /* Check the ILBM file */
    if(!ILBM_checkImages(chunk, images, imagesLength))
    {
        fprintf(stderr, "ILBM file is not valid!\n");
        ILBM_freeImages(images, imagesLength);
        ILBM_free(chunk);
        return 1;
    }
    
    if(imagesLength == 0)
    {
        fprintf(stderr, "There are no (valid) ILBM images in the IFF file!\n");
        ILBM_freeImages(images, imagesLength);
        ILBM_free(chunk);
        return 1;
    }
    
    if(number > imagesLength - 1)
    {
        fprintf(stderr, "Image with index: %d does not exist. Valid ranges are: 0 - %d\n", number, imagesLength - 1);
        ILBM_freeImages(images, imagesLength);
        ILBM_free(chunk);
        return 1;
    }
    
    /* Initialize video subsystem */
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "Error initialising SDL video subsystem, reason: %s\n", SDL_GetError());
        ILBM_freeImages(images, imagesLength);
        ILBM_free(chunk);
        return 1;
    }
    
    /* Main loop */
    while(status != SDL_ILBM_STATUS_QUIT && status != SDL_ILBM_STATUS_ERROR)
    {
        ILBM_Image *image = images[number];
        status = viewILBMImage(image, format, number, imagesLength, lowresPixelScaleFactor, options);
        
        switch(status)
        {
            case SDL_ILBM_STATUS_PREVIOUS:
                number--;
                break;
            case SDL_ILBM_STATUS_NEXT:
                number++;
                break;
        }
    }
    
    /* Cleanup */
    ILBM_freeImages(images, imagesLength);
    ILBM_free(chunk);
    SDL_Quit();
    
    /* Return the exit status */
    return (status == SDL_ILBM_STATUS_ERROR);
}
