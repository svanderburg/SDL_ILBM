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
 
#ifndef _MSC_VER
#include <getopt.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "viewer.h"

static void printUsage(const char *command)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "%s [options] file.ILBM\n", command);

#ifdef _MSC_VER
	fprintf(stderr, "/f    Specify the output format. Possible values are: auto, chunky or rgb. Defaults to: auto\n");
	fprintf(stderr, "/C    Turn cycle mode on by default\n");
	fprintf(stderr, "/s    Do not clip the picture inside the page but stretch it to its full size\n");\
	fprintf(stderr, "/c    Specifies the scale factor of a lowres pixel to properly correct its aspect ratio. Possible values are: auto, none, 2, 4\n");
	fprintf(stderr, "/F    Views the picture in full screen\n");
	fprintf(stderr, "/n    Displays the n-th picture inside the IFF scrap file. Defaults to: 0\n");
#else
    fprintf(stderr, "-f,--format         Specify the output format. Possible values are: auto, chunky or rgb. Defaults to: auto\n");
    fprintf(stderr, "-C,--cyle           Turn cycle mode on by default\n");
    fprintf(stderr, "-s,--stretch        Do not clip the picture inside the page but stretch it to its full size\n");\
    fprintf(stderr, "-c,--correct-aspect Specifies the scale factor of a lowres pixel to properly correct its aspect ratio. Possible values are: auto, none, 2, 4\n");
    fprintf(stderr, "-F,--fullscreen     Views the picture in full screen\n");
    fprintf(stderr, "-n,--number         Displays the n-th picture inside the IFF scrap file. Defaults to: 0\n");
#endif
}

static SDL_ILBM_Format determineFormat(const char *format)
{
	if (strcmp(format, "auto") == 0)
		return SDL_ILBM_FORMAT_AUTO;
	else if (strcmp(format, "chunky") == 0)
		return SDL_ILBM_FORMAT_CHUNKY;
	else if (strcmp(format, "rgb") == 0)
		return SDL_ILBM_FORMAT_RGB;
	else
		return SDL_ILBM_FORMAT_AUTO;
}

static unsigned int determineLowresPixelScaleFactor(const char *factor)
{
	if (strcmp(factor, "auto") == 0)
		return 0;
	else if (strcmp(factor, "none") == 0)
		return 1;
	else if (strcmp(factor, "2") == 0)
		return 2;
	else if (strcmp(factor, "4") == 0)
		return 4;
	else
		return 0;
}

int main(int argc, char *argv[])
{
	SDL_ILBM_Format format = SDL_ILBM_FORMAT_AUTO;
	unsigned int lowresPixelScaleFactor = 0;
	unsigned int number = 0;
	unsigned int options = 0;

#ifdef _MSC_VER
	unsigned int optind = 1;
	unsigned int i; 

	int formatFollows = FALSE;
	int lowresPixelScaleFactorFollows = FALSE;
	int numberFollows = FALSE;

	for (i = 1; i < argc; i++)
	{
		if (formatFollows)
		{
			formatFollows = FALSE;
			format = determineFormat(argv[i]);
			optind++;
		}
		else if (lowresPixelScaleFactorFollows)
		{
			lowresPixelScaleFactorFollows = FALSE;
			lowresPixelScaleFactor = determineLowresPixelScaleFactor(argv[i]);
			optind++;
		}
		else if (numberFollows)
		{
			numberFollows = FALSE;
			number = atoi(argv[i]);
			optind++;
		}
		else if (strcmp(argv[i], "/f") == 0)
		{
			formatFollows = TRUE;
			optind++;
		}
		else if(strcmp(argv[i], "/C") == 0)
		{
			options |= SDL_ILBM_OPTION_CYCLE;
			optind++;
		}
		else if(strcmp(argv[i], "/s") == 0)
		{
			options |= SDL_ILBM_OPTION_STRETCH;
			optind++;
		}
		else if (strcmp(argv[i], "/c") == 0)
		{
			lowresPixelScaleFactorFollows = TRUE;
			optind++;
		}
		else if (strcmp(argv[i], "/F") == 0)
		{
			options |= SDL_ILBM_OPTION_FULLSCREEN;
			optind++;
		}
		else if (strcmp(argv[i], "/n") == 0)
		{
			numberFollows = TRUE;
			optind++;
		}
		else if (strcmp(argv[i], "/?") == 0)
		{
			printUsage(argv[0]);
			return 0;
		}  
	}
#else
    int c, option_index = 0;
    struct option long_options[] =
    {
	{"format", required_argument, 0, 'f'},
	{"cycle", no_argument, 0, 'C'},
	{"stretch", no_argument, 0, 's'},
	{"correct-aspect", required_argument, 0, 'c'},
	{"fullscreen", no_argument, 0, 'F'},
	{"number", required_argument, 0, 'n'},
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
    };

    /* Parse command-line options */
    while((c = getopt_long(argc, argv, "f:Csc:Fn:h", long_options, &option_index)) != -1)
    {
	switch(c)
	{
	    case 'f':
			format = determineFormat(optarg);
		break;
	    case 'C':
		options |= SDL_ILBM_OPTION_CYCLE;
		break;
	    case 's':
		options |= SDL_ILBM_OPTION_STRETCH;
		break;
	    case 'c':
			lowresPixelScaleFactor = determineLowresPixelScaleFactor(optarg);
		break;
	    case 'F':
		options |= SDL_ILBM_OPTION_FULLSCREEN;
		break;
	    case 'n':
		number = atoi(optarg);
		break;
	    case 'h':
	    case '?':
		printUsage(argv[0]);
		return 0;
	}
    }
#endif

    /* Validate non options */
    
	if (optind >= argc)
	{
		fprintf(stderr, "ERROR: No ILBM file given!\n");
		return 1;
	}
	else
		return SDL_ILBM_viewILBMImages(argv[optind], format, number, lowresPixelScaleFactor, options);
}
