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
    printf("Usage: %s [OPTION] [file.ILBM]\n\n", command);

    puts(
    "View a collection of ILBM images inside an IFF file. If no ILBM file is given it reads\n"
    "from the standard input.\n"
    );

    puts(
    "Options:\n"
#ifdef _MSC_VER
    "  /f FORMAT  Specify the output format. Possible values are: auto, chunky or rgb.\n"
    "             Defaults to: auto\n"
    "  /C         Turn cycle mode on by default\n"
    "  /s         Do not clip the picture inside the page but stretch it to its full\n"
    "             size"
    );
    puts(
    "  /c VALUE   Specifies the scale factor of a lowres pixel to properly correct\n"
    "             its aspect ratio. Possible values are: auto, none, 2, 4\n"
    "  /F         Views the picture in full screen\n"
    );
    "  /n NUM     Displays the n-th picture inside the IFF scrap file. Defaults to: 0\n"
    "  /?         Shows the usage of the command to the user\n"
    "  /v         Shows the version of the command to the user\n"
#else
    "  -f, --format=FORMAT         Specify the output format. Possible values are:\n"
    "                              auto, chunky or rgb. Defaults to: auto\n"
    "  -C, --cycle                 Turn cycle mode on by default\n"
    "  -s, --stretch               Do not clip the picture inside the page but\n"
    "                              stretch it to its full size"
    );
    puts(
    "  -c, --correct-aspect=VALUE  Specifies the scale factor of a lowres pixel to\n"
    "                              properly correct its aspect ratio. Possible values\n"
    "                              are: auto, none, 2, 4\n"
    "  -F, --fullscreen            Views the picture in full screen"
    );
    puts(
    "  -n, --number=NUM            Displays the n-th picture inside the IFF scrap\n"
    "                              file. Defaults to: 0\n"
    "  -h, --help                  Shows the usage of the command to the user\n"
    "  -v, --version               Shows the version of the command to the user"
#endif
    );
}

static void printVersion(const char *command)
{
    printf(
    "%s (" PACKAGE_NAME ") " PACKAGE_VERSION "\n\n"
    "Copyright (C) 2012-2015 Sander van der Burg\n"
    , command);
}

static SDL_ILBM_Format determineFormat(const char *format)
{
    if (strcmp(format, "auto") == 0)
        return SDL_ILBM_AUTO_FORMAT;
    else if (strcmp(format, "chunky") == 0)
        return SDL_ILBM_CHUNKY_FORMAT;
    else if (strcmp(format, "rgb") == 0)
        return SDL_ILBM_RGB_FORMAT;
    else
        return SDL_ILBM_AUTO_FORMAT;
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
    SDL_ILBM_Format format = SDL_ILBM_AUTO_FORMAT;
    unsigned int lowresPixelScaleFactor = 0;
    unsigned int number = 0;
    unsigned int options = 0;
    char *filename;

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
        else if (strcmp(argv[i], "/v") == 0)
        {
            printVersion(argv[0]);
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
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    /* Parse command-line options */
    while((c = getopt_long(argc, argv, "f:Csc:Fn:hv", long_options, &option_index)) != -1)
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
            case 'v':
                printVersion(argv[0]);
                return 0;
        }
    }
#endif

    /* Validate non options */

    if (optind >= argc)
        filename = NULL;
    else
        filename = argv[optind];

    return SDL_ILBM_viewILBMImages(filename, format, number, lowresPixelScaleFactor, options);
}
