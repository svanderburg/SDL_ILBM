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

#include "cycle.h"
#include <stdlib.h>

#define _60_STEPS 60.0
#define MILLIS_PER_SECOND 1000
#define MICROS_PER_MILLIS 1000

static void shiftColorRange(amiVideo_Palette *palette, const ILBM_ColorRange *colorRange, int shiftRight)
{
    unsigned int i;
    amiVideo_Color *color = palette->bitplaneFormat.color;

    if(shiftRight)
    {
        amiVideo_Color temp = color[colorRange->low];

        for(i = colorRange->low; i < colorRange->high; i++)
            color[i] = color[i + 1];

        color[colorRange->high] = temp;
    }
    else
    {
        amiVideo_Color temp = color[colorRange->high];

        for(i = colorRange->high; i > colorRange->low; i--)
            color[i] = color[i - 1];

        color[colorRange->low] = temp;
    }
}

static void shiftDRange(amiVideo_Palette *palette, const ILBM_DRange *drange)
{
    unsigned int i;
    amiVideo_Color *color = palette->bitplaneFormat.color;
    amiVideo_Color temp = color[drange->dindex[drange->min].index];

    for(i = drange->min; i < drange->max; i++)
        color[drange->dindex[i].index] = color[drange->dindex[i + 1].index];

    color[drange->dindex[drange->max].index] = temp;
}

static void shiftCycleInfo(amiVideo_Palette *palette, const ILBM_CycleInfo *cycleInfo)
{
    amiVideo_Color *color = palette->bitplaneFormat.color;

    if(cycleInfo->direction == ILBM_CYCLEINFO_SHIFT_LEFT)
    {
        /* Shift left */
        unsigned int i;
        amiVideo_Color temp = color[cycleInfo->end];

        for(i = cycleInfo->end; i > cycleInfo->start; i--)
            color[i] = color[i - 1];

        color[cycleInfo->start] = temp;
    }
    else if(cycleInfo->direction == ILBM_CYCLEINFO_SHIFT_RIGHT)
    {
        /* Shift right */
        unsigned int i;
        amiVideo_Color temp = color[cycleInfo->start];

        for(i = cycleInfo->start; i < cycleInfo->end; i++)
            color[i] = color[i + 1];

        color[cycleInfo->end] = temp;
    }
}

static Uint32 computeColorRangeTime(const Uint32 ticks, const ILBM_ColorRange *colorRange)
{
    return (Uint32)(MILLIS_PER_SECOND / (_60_STEPS * colorRange->rate / ILBM_COLORRANGE_60_STEPS_PER_SECOND) + ticks);
}

static Uint32 computeDRangeTime(const Uint32 ticks, const ILBM_DRange *drange)
{
    return (Uint32)(MILLIS_PER_SECOND / (_60_STEPS * drange->rate / ILBM_DRANGE_60_STEPS_PER_SECOND) + ticks);
}

static Uint32 computeCycleInfoTime(const Uint32 ticks, const ILBM_CycleInfo *cycleInfo)
{
    return (Uint32)(MILLIS_PER_SECOND * cycleInfo->seconds + cycleInfo->microSeconds / MICROS_PER_MILLIS + ticks);
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

void SDL_ILBM_cleanupRangeTimes(SDL_ILBM_RangeTimes *rangeTimes)
{
    free(rangeTimes->crngTimes);
    free(rangeTimes->drngTimes);
    free(rangeTimes->ccrtTimes);
}

void SDL_ILBM_shiftActiveRanges(SDL_ILBM_RangeTimes *rangeTimes, const ILBM_Image *image, amiVideo_Palette *palette)
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
            shiftColorRange(palette, colorRange, colorRange->active & ILBM_COLORRANGE_SHIFT_RIGHT);
            crngTimes[i] = computeColorRangeTime(ticks, colorRange); /* Update time */
        }
    }

    for(i = 0; i < image->drangeLength; i++)
    {
        ILBM_DRange *drange = image->drange[i];

        if((drange->flags & ILBM_RNG_ACTIVE) == ILBM_RNG_ACTIVE && ticks >= drngTimes[i])
        {
            shiftDRange(palette, drange);
            drngTimes[i] = computeDRangeTime(ticks, drange); /* Update time */
        }
    }

    for(i = 0; i < image->cycleInfoLength; i++)
    {
        ILBM_CycleInfo *cycleInfo = image->cycleInfo[i];

        if(cycleInfo->direction != 0 && ticks >= ccrtTimes[i])
        {
            shiftCycleInfo(palette, cycleInfo);
            ccrtTimes[i] = computeCycleInfoTime(ticks, cycleInfo); /* Update time */
        }
    }
}
