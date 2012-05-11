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
 
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include "viewer.h"

static void printUsage(const char *command)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s [options] file.ILBM\n", command);
    fprintf(stderr, "--colormode    Specify the colormode\n");
}

int main(int argc, char *argv[])
{
    /* Declarations */
    int c, option_index = 0;
    struct option long_options[] =
    {
	{"colormode", required_argument, 0, 'c'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
    };

    SDL_ILBM_ColorMode colorMode = SDL_ILBM_COLOR_AUTO;
    
    /* Parse command-line options */
    while((c = getopt_long(argc, argv, "c:h", long_options, &option_index)) != -1)
    {
	switch(c)
	{
	    case 'c':
		if(strcmp(optarg, "auto") == 0)
		    colorMode = SDL_ILBM_COLOR_AUTO;
		else if(strcmp(optarg, "index8") == 0)
		    colorMode = SDL_ILBM_COLOR_INDEX8;
		else if(strcmp(optarg, "rgb") == 0)
		    colorMode = SDL_ILBM_COLOR_RGB;
		break;
	    case 'h':
	    case '?':
		printUsage(argv[0]);
		return 0;
	}
    }
    
    /* Validate non options */
    
    if(optind >= argc)
    {
	fprintf(stderr, "ERROR: No ILBM file given!\n");
	return 1;
    }
    else
	return SDL_ILBM_viewILBMImage(argv[optind], colorMode);
}
