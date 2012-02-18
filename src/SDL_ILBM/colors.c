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

#include "colors.h"
#include <palette.h>
#include <viewportmode.h>

SDL_Color* SDL_ILBM_computeColors(const ILBM_Image *image, unsigned int *colorsLength)
{
    IFF_Long viewportMode;
    
    if(image->viewport == NULL)
	viewportMode = 0;
    else
	viewportMode = image->viewport->viewportMode;
	
    return (SDL_Color*)amiVideo_computePalette((AMI_Color*)image->colorMap->colorRegister, image->colorMap->colorRegisterLength, viewportMode, colorsLength);
}

SDL_ILBM_ColorMode SDL_ILBM_autoPickColorMode(const ILBM_Image *image, SDL_ILBM_ColorMode colorMode)
{
    if(colorMode == SDL_ILBM_COLOR_AUTO)
    {
        if(image->viewport == NULL || !amiVideo_checkHoldAndModify(image->viewport->viewportMode))
	    return SDL_ILBM_COLOR_INDEX8;
	else
	    return SDL_ILBM_COLOR_RGB;
    }
    else
        return colorMode;
}
