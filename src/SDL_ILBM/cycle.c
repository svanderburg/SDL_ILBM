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

#include "cycle.h"
#include <stdlib.h>

static void shiftColorRange(SDL_Color *colors, const ILBM_ColorRange *colorRange)
{
    unsigned int i;
    SDL_Color temp = colors[colorRange->low];

    for(i = colorRange->low; i < colorRange->high; i++)
	colors[i] = colors[i + 1];

    colors[colorRange->high] = temp;
}

static void shiftDRange(SDL_Color *colors, const ILBM_DRange *drange)
{
    unsigned int i;
    SDL_Color temp = colors[drange->dindex[drange->min].index];

    for(i = drange->min; i < drange->max; i++)
	colors[drange->dindex[i].index] = colors[drange->dindex[i + 1].index];

    colors[drange->dindex[drange->max].index] = temp;
}

static void shiftCycleInfo(SDL_Color *colors, const ILBM_CycleInfo *cycleInfo)
{
    if(cycleInfo->direction == -1)
    {
	/* Shift left */
	unsigned int i;
	SDL_Color temp = colors[cycleInfo->end];
	
	for(i = cycleInfo->end; i > cycleInfo->start; i--)
	    colors[i] = colors[i - 1];
	
	colors[cycleInfo->start] = temp;
    }
    else if(cycleInfo->direction == 1)
    {
	/* Shift right */
	unsigned int i;
	SDL_Color temp = colors[cycleInfo->start];
	
	for(i = cycleInfo->start; i < cycleInfo->end; i++)
	    colors[i] = colors[i + 1];
	
	colors[cycleInfo->end] = temp;
    }
}

static Uint32 computeColorRangeTime(const Uint32 ticks, const ILBM_ColorRange *colorRange)
{
    return 1000 / (60.0 * colorRange->rate / ILBM_COLORRANGE_MAX_ACTIVE_VALUE) + ticks;
}

static Uint32 computeDRangeTime(const Uint32 ticks, const ILBM_DRange *drange)
{
    return 1000 / (60.0 * drange->rate / ILBM_DRANGE_MAX_ACTIVE_VALUE) + ticks;
}

static Uint32 computeCycleInfoTime(const Uint32 ticks, const ILBM_CycleInfo *cycleInfo)
{
    return 1000 * cycleInfo->seconds + cycleInfo->microSeconds / 1000 + ticks;
}

void SDL_ILBM_initRangeTimes(SDL_ILBM_RangeTimes *rangeTimes, const ILBM_Image *image)
{
    unsigned int i;
    Uint32 ticks; 
    
    rangeTimes->crngTimes = (Uint32*)malloc(image->colorRangeLength * sizeof(Uint32));
    rangeTimes->drngTimes = (Uint32*)malloc(image->drangeLength * sizeof(Uint32));
    rangeTimes->ccrtTimes = (Uint32*)malloc(image->cycleInfoLength * sizeof(Uint32));
    
    ticks = SDL_GetTicks();
    
    for(i = 0; i < image->colorRangeLength; i++)
        rangeTimes->crngTimes[i] = computeColorRangeTime(ticks, image->colorRange[i]);
    
    for(i = 0; i < image->drangeLength; i++)
        rangeTimes->drngTimes[i] = computeDRangeTime(ticks, image->drange[i]);
    
    for(i = 0; i < image->cycleInfoLength; i++)
        rangeTimes->ccrtTimes[i] = computeCycleInfoTime(ticks, image->cycleInfo[i]);
}

void SDL_ILBM_shiftActiveRanges(SDL_ILBM_RangeTimes *rangeTimes, const ILBM_Image *image, SDL_Color *colors)
{
    unsigned int i;
    Uint32 *crngTimes = rangeTimes->crngTimes;
    Uint32 *drngTimes = rangeTimes->drngTimes;
    Uint32 *ccrtTimes = rangeTimes->ccrtTimes;
    Uint32 ticks = SDL_GetTicks();
    
    for(i = 0; i < image->colorRangeLength; i++)
    {
	ILBM_ColorRange *colorRange = image->colorRange[i];
	
	if(colorRange->active != 0 && ticks >= crngTimes[i])
	{
	    shiftColorRange(colors, colorRange);
	    
	    /* Update time */
	    crngTimes[i] = computeColorRangeTime(ticks, colorRange);
	}
    }

    for(i = 0; i < image->drangeLength; i++)
    {
	ILBM_DRange *drange = image->drange[i];
	
	if(drange->flags == ILBM_RNG_ACTIVE && ticks >= drngTimes[i])
	{
	    shiftDRange(colors, drange);
		    
	    /* Update time */
	    drngTimes[i] = computeDRangeTime(ticks, drange);
	}
    }
    
    for(i = 0; i < image->cycleInfoLength; i++)
    {
	ILBM_CycleInfo *cycleInfo = image->cycleInfo[i];
	
	if(cycleInfo->direction != 0 && ticks >= ccrtTimes[i])
	{
	    shiftCycleInfo(colors, cycleInfo);
	    
	    /* Update time */
	    ccrtTimes[i] = computeCycleInfoTime(ticks, cycleInfo);
	}
    }
}

void SDL_ILBM_freeRangeTimes(SDL_ILBM_RangeTimes *rangeTimes)
{
    free(rangeTimes->crngTimes);
    free(rangeTimes->drngTimes);
    free(rangeTimes->ccrtTimes);
}

void SDL_ILBM_restorePalette(const SDL_Color *originalColors, SDL_Color *colors, const unsigned int colorsLength)
{
    memcpy(colors, originalColors, colorsLength * sizeof(SDL_Color));
}
