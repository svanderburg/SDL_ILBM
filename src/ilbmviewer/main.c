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

    printf("View a collection of ILBM images inside an IFF file. If no ILBM file is given it reads\n");
    printf("from the standard input.\n\n");
    
    printf("Options:\n");
#ifdef _MSC_VER
    printf("  /f FORMAT  Specify the output format. Possible values are: auto, chunky or rgb.\n");
    printf("              Defaults to: auto\n");
    printf("  /C         Turn cycle mode on by default\n");
    printf("  /s         Do not clip the picture inside the page but stretch it to its full\n");
    printf("             size\n");
    printf("  /c VALUE   Specifies the scale factor of a lowres pixel to properly correct\n");
    printf("             its aspect ratio. Possible values are: auto, none, 2, 4\n");
    printf("  /F         Views the picture in full screen\n");
    printf("  /n NUM     Displays the n-th picture inside the IFF scrap file. Defaults to: 0\n");
    printf("  /?         Shows the usage of the command to the user\n");
    printf("  /v         Shows the version of the command to the user\n");
#else
    printf("  -f, --format=FORMAT         Specify the output format. Possible values are:\n");
    printf("                              auto, chunky or rgb. Defaults to: auto\n");
    printf("  -C, --cycle                 Turn cycle mode on by default\n");
    printf("  -s, --stretch               Do not clip the picture inside the page but\n");
    printf("                              stretch it to its full size\n");\
    printf("  -c, --correct-aspect=VALUE  Specifies the scale factor of a lowres pixel to\n");
    printf("                              properly correct its aspect ratio. Possible values\n");
    printf("                              are: auto, none, 2, 4\n");
    printf("  -F, --fullscreen            Views the picture in full screen\n");
    printf("  -n, --number=NUM            Displays the n-th picture inside the IFF scrap\n");
    printf("                              file. Defaults to: 0\n");
    printf("  -h, --help                  Shows the usage of the command to the user\n");
    printf("  -v, --version               Shows the version of the command to the user\n");
#endif
}

static void printVersion(const char *command)
{
    printf("%s (" PACKAGE_NAME ") " PACKAGE_VERSION "\n\n", command);
    printf("Copyright (C) 2012-2015 Sander van der Burg\n");
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
