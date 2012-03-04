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

#include "viewer.h"
#include <stdio.h>
#include <SDL.h>
#include <ilbm.h>
#include <ilbmimage.h>
#include <viewportmode.h>
#include "window.h"
#include "image.h"
#include "cycle.h"

int SDL_ILBM_viewILBMImage(const char *filename, const SDL_ILBM_ColorMode colorMode)
{
    IFF_Chunk *chunk;
    ILBM_Image **images;
    unsigned int imagesLength;
    int quit = FALSE;
    unsigned int imageNumber = 0;
    
    /* Initialize video subsystem */
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
	fprintf(stderr, "Error initialising SDL video subsystem, reason: %s\n", SDL_GetError());
	return 1;
    }
    
    /* Enable keyboard repeat */
    if(SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL) < 0)
	fprintf(stderr, "Cannot enable keyboard repeat!\n");
    
    /* Open the ILBM file */
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
	return 1;
    }
    
    if(imagesLength == 0)
    {
	fprintf(stderr, "Error extracting ILBM images from IFF file!\n");
	return 1;
    }
    
    while(!quit)
    {
	SDL_ILBM_Window window;
	SDL_Surface *picture, *screen;
	SDL_ILBM_RangeTimes rangeTimes;
	SDL_Color *colors = NULL, *originalColors = NULL;
	unsigned int colorsLength;
	int cycle = FALSE;
	int switchPicture = FALSE;
	SDL_ILBM_ColorMode currentColorMode;
	int xOffset = 0, yOffset = 0;
	int fullscreen = 0;
	int stretch = FALSE;
	
	ILBM_Image *image = images[imageNumber];
    
	/* Determine color mode */
	
	currentColorMode = SDL_ILBM_autoPickColorMode(image, colorMode);
	
	/* Create picture surface */
    
	if(currentColorMode == SDL_ILBM_COLOR_INDEX8)
	{
	    colors = SDL_ILBM_computeColors(image, &colorsLength);
	    picture = SDL_ILBM_createChunkySurface(image, colors, colorsLength);
	    
	    originalColors = (SDL_Color*)malloc(colorsLength * sizeof(SDL_Color));
	    SDL_ILBM_restorePalette(colors, originalColors, colorsLength);
	}
	else
	    picture = SDL_ILBM_createRGBASurface(image);
    
	/* Initialize window surface */
    
	SDL_ILBM_initWindow(&window, image);
	
	screen = SDL_ILBM_updateWindowSettings(&window, fullscreen, stretch);
    
	if(screen == NULL)
	{
	    fprintf(stderr, "Couldn't set video mode: %s\n", SDL_GetError());
	    return 1;
	}
	
	/* Set window title */
	SDL_WM_SetCaption("SDL ILBM Viewer", "ilbmviewer");
    
	/* Blit the picture on the surface */
	SDL_ILBM_drawPicture(picture, screen, 0, 0);
	
	/* Initialize cycle times */
	SDL_ILBM_initRangeTimes(&rangeTimes, image);
	
	/* Main loop */
	
	while(!quit && !switchPicture)
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
				if(fullscreen)
				    fullscreen = 0;
				else
				    fullscreen = SDL_FULLSCREEN;
				
				screen = SDL_ILBM_updateWindowSettings(&window, fullscreen, stretch);
				SDL_ILBM_drawPicture(picture, screen, xOffset, yOffset);
				break;
			
			    case SDLK_s:
				stretch = !stretch;
				
				xOffset = 0;
				yOffset = 0;
				
				screen = SDL_ILBM_updateWindowSettings(&window, fullscreen, stretch);
				SDL_ILBM_drawPicture(picture, screen, xOffset, yOffset);
				break;
			
			    case SDLK_LEFT:
				if(xOffset > 0)
				    xOffset--;
				
				SDL_ILBM_drawPicture(picture, screen, xOffset, yOffset);
				break;
				
			    case SDLK_RIGHT:
				if(xOffset < image->bitMapHeader->w - window.width)
				    xOffset++;
				
				SDL_ILBM_drawPicture(picture, screen, xOffset, yOffset);
				break;
			    
			    case SDLK_UP:
				if(yOffset > 0)
				    yOffset--;
				
				SDL_ILBM_drawPicture(picture, screen, xOffset, yOffset);
				break;
			
			    case SDLK_DOWN:
				if(yOffset < image->bitMapHeader->h - window.height)
				    yOffset++;
				
				SDL_ILBM_drawPicture(picture, screen, xOffset, yOffset);
				break;
			    
			    case SDLK_SPACE:
			    case SDLK_PAGEDOWN:
				if(imageNumber < imagesLength - 1)
				    imageNumber++;
				
				switchPicture = TRUE;
				break;
			
			    case SDLK_PAGEUP:
				if(imageNumber > 0)
				    imageNumber--;
				
				switchPicture = TRUE;
				break;
				
			    case SDLK_ESCAPE:
				quit = TRUE;
				break;
			    
			    case SDLK_TAB:
				if(currentColorMode == SDL_ILBM_COLOR_INDEX8)
				{
				    cycle = !cycle;
			    
				    /* Restore the original color palette */
				    if(!cycle)
				    {
					SDL_ILBM_restorePalette(originalColors, colors, colorsLength);
					SDL_ILBM_updatePalette(colors, colorsLength, picture, screen, xOffset, yOffset);
				    }
				}
				break;
			}
			break;
		    
		    case SDL_QUIT:
			quit = TRUE;
			break;
		}
	    }
	
	    if(cycle)
	    {
		SDL_ILBM_shiftActiveRanges(&rangeTimes, image, colors);
		SDL_ILBM_updatePalette(colors, colorsLength, picture, screen, xOffset, yOffset);
	    }
	
	    if(SDL_Flip(screen) < 0)
	    {
		fprintf(stderr, "SDL flip failed: %s!\n", SDL_GetError());
		return 1;
	    }
	}
    
	/* Clean up */
    
	SDL_FreeSurface(picture);
	SDL_ILBM_freeRangeTimes(&rangeTimes);
	free(originalColors);
	free(colors);
    }
    
    ILBM_freeImages(images, imagesLength);
    ILBM_free(chunk);
    
    SDL_Quit();
    
    return 0;
}
