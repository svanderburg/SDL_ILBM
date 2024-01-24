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
#include <set.h>
#include "image.h"
#include "cycle.h"
#include "viewerdisplay.h"

typedef enum
{
    SDL_ILBM_STATUS_NONE,
    SDL_ILBM_STATUS_QUIT,
    SDL_ILBM_STATUS_PREVIOUS,
    SDL_ILBM_STATUS_NEXT,
    SDL_ILBM_STATUS_ERROR
}
SDL_ILBM_Status;

static SDL_ILBM_Status viewILBMImage(SDL_ILBM_Set *set, const unsigned int number, const unsigned int lowresPixelScaleFactor, const SDL_ILBM_Format format, const unsigned int options)
{
    int cycle, fullscreen, stretch, status = SDL_ILBM_STATUS_NONE;
    SDL_ILBM_ViewerDisplay viewerDisplay;

    SDL_ILBM_Image *image = SDL_ILBM_createImageFromSet(set, number, lowresPixelScaleFactor, format);

    if(image == NULL)
    {
        fprintf(stderr, "Cannot open image: %d\n", number);
        return SDL_ILBM_STATUS_ERROR;
    }

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

    /* Initialize everything window related */
    if(!SDL_ILBM_initViewerDisplay(&viewerDisplay, image, stretch, fullscreen))
    {
        fprintf(stderr, "Cannot init viewer display!\n");
        status = SDL_ILBM_STATUS_ERROR;
    }

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
                            SDL_ILBM_destroyViewerDisplay(&viewerDisplay);

                            fullscreen = !fullscreen;

                            if(!SDL_ILBM_initViewerDisplay(&viewerDisplay, image, stretch, fullscreen))
                                status = SDL_ILBM_STATUS_ERROR;
                            break;

                        case SDLK_s:
                            SDL_ILBM_destroyViewerDisplay(&viewerDisplay);

                            stretch = !stretch;

                            if(!SDL_ILBM_initViewerDisplay(&viewerDisplay, image, stretch, fullscreen))
                                status = SDL_ILBM_STATUS_ERROR;
                            break;

                        case SDLK_ESCAPE:
                            status = SDL_ILBM_STATUS_QUIT;
                            break;

                        case SDLK_SPACE:
                        case SDLK_PAGEDOWN:
                            if(number < set->imagesLength - 1)
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
                                /* If we stop cycling, we reset the colors back to normal */
                                SDL_ILBM_resetColors(image);

                                if(!SDL_ILBM_renderTexture(&viewerDisplay))
                                    status = SDL_ILBM_STATUS_ERROR;
                            }
                            break;

                        case SDLK_LEFT:
                            if(!SDL_ILBM_scrollWindowLeft(&viewerDisplay))
                                status = SDL_ILBM_STATUS_QUIT;
                            break;

                        case SDLK_RIGHT:
                            if(!SDL_ILBM_scrollWindowRight(&viewerDisplay))
                                status = SDL_ILBM_STATUS_QUIT;
                            break;

                        case SDLK_UP:
                            if(!SDL_ILBM_scrollWindowUp(&viewerDisplay))
                                status = SDL_ILBM_STATUS_QUIT;
                            break;

                        case SDLK_DOWN:
                            if(!SDL_ILBM_scrollWindowDown(&viewerDisplay))
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
            SDL_ILBM_cycleColors(image);

            if(!SDL_ILBM_renderTexture(&viewerDisplay))
                status = SDL_ILBM_STATUS_ERROR;
        }

        /* Flip screen buffers, so that changes become visible */
        SDL_RenderPresent(viewerDisplay.renderer);
    }

    /* Cleanup */
    SDL_ILBM_destroyViewerDisplay(&viewerDisplay);
    SDL_ILBM_freeImage(image);

    /* Return exit status */
    return status;
}

int SDL_ILBM_viewILBMImages(const char *filename, const SDL_ILBM_Format format, unsigned int number, const unsigned int lowresPixelScaleFactor, const unsigned int options)
{
    SDL_ILBM_Status status = SDL_ILBM_STATUS_NONE;
    SDL_ILBM_Set *set;

    if(filename == NULL)
        set = SDL_ILBM_createSetFromFd(stdin);
    else
        set = SDL_ILBM_createSetFromFilename(filename);

    if(set == NULL)
    {
        fprintf(stderr, "Error parsing ILBM file!\n");
        return 1;
    }

    if(number > set->imagesLength)
    {
        fprintf(stderr, "Image with index: %d does not exist. Valid ranges are: 0 - %d\n", number, set->imagesLength - 1);
        SDL_ILBM_freeSet(set);
        return 1;
    }

    /* Initialize video subsystem */
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "Error initialising SDL video subsystem, reason: %s\n", SDL_GetError());
        SDL_ILBM_freeSet(set);
        return 1;
    }

    /* Main loop */
    while(status != SDL_ILBM_STATUS_QUIT && status != SDL_ILBM_STATUS_ERROR)
    {
        status = viewILBMImage(set, number, lowresPixelScaleFactor, format, options);

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
    SDL_ILBM_freeSet(set);
    SDL_Quit();

    /* Return the exit status */
    return (status == SDL_ILBM_STATUS_ERROR);
}
