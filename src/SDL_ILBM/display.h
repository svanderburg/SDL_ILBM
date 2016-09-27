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

#ifndef __SDL_ILBM_DISPLAY_H
#define __SDL_ILBM_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SDL_ILBM_Display SDL_ILBM_Display;

#include "image.h"

/**
 * @brief Encapsulates a window in which an image is being displayed
 */
struct SDL_ILBM_Display
{
    /** Reference to the SDL_ILBM_Image that is supposed to be displayed */
    const SDL_ILBM_Image *image;
    
    /** The width of the display (corresponds to the page width or the image width) */
    int width;
    
    /** The height of the display (corresponds to the page height or the image height) */
    int height;
    
    /** A surface used for converting the image to a 32-bit surface format so that it can blitted to a texture */
    SDL_Surface *blitSurface;
    
    /** Indicates whether the blit surface must be freed while freeing the SDL_ILBM_Display struct */
    int mustFreeBlitSurface;
};

/**
 * Initializes a preallocated display with an image to display.
 *
 * @param display Preallocated display struct
 * @param image SDL_ILBM_Image to display in the window
 * @param strech TRUE to indicate the display's dimensions should correspond to the image, FALSE to indicate the display's dimensions should correspond to the page
 * @param return TRUE if the initialization succeeds, else FALSE
 */
int SDL_ILBM_initDisplay(SDL_ILBM_Display *display, const SDL_ILBM_Image *image, const int stretch);

/**
 * Creates a display window for displaying a provided image.
 *
 * @param image SDL_ILBM_Image to display in the window
 * @param strech TRUE to indicate the display's dimensions should correspond to the image, FALSE to indicate the display's dimensions should correspond to the page
 * @return An SDL_ILBM_Display struct or NULL in case of an error. The result should be freed by invoking SDL_ILBM_freeDisplay()
 */
SDL_ILBM_Display *SDL_ILBM_createDisplay(const SDL_ILBM_Image *image, const int stretch);

/**
 * Removes the display properties from memory
 *
 * @param display An SDL_ILBM_Display instance
 */
void SDL_ILBM_destroyDisplay(SDL_ILBM_Display *display);

/**
 * Frees a display from memory
 *
 * @param display An SDL_ILBM_Display instance
 */
void SDL_ILBM_freeDisplay(SDL_ILBM_Display *display);

/**
 * Constructs an SDL_Window from a provided display. This function is basically
 * a wrapper around SDL's SDL_CreateWindow().
 *
 * @param title Title of the window
 * @param x X coordinate of the window
 * @param y X coordinate of the window
 * @param display An SDL_ILBM_Display instance
 * @param flags SDL window flags
 * @return An SDL window or NULL in case of an error. The resulting window must be freed by invoking SDL_DestroyWindow()
 */
SDL_Window *SDL_ILBM_createWindow(const char *title, int x, int y, const SDL_ILBM_Display *display, Uint32 flags);

/**
 * Sets the logical size of a renderer to the same dimensions as the display.
 * This function is basically a wrapper around SDL's SDL_RenderSetLogicalSize()
 *
 * @param renderer An SDL_Renderer instance
 * @param display An SDL_ILBM_Display instance
 * @return 0 in case of success, else a value below zero
 */
int SDL_ILBM_renderSetLogicalSize(SDL_Renderer *renderer, const SDL_ILBM_Display *display);

/**
 * Construct a texture that can be used for hardware rendering from a display.
 * This function is basically a wrapper around SDL's SDL_CreateTexture()
 *
 * @param renderer An SDL_Renderer instance
 * @param format One of the enumerated SDL texture formats
 * @param access One of the enumerated SDL access modes
 * @param display An SDL_ILBM_Display instance
 * @return An SDL texture or NULL in case of an error. The resulting texture must be freed by invoking SDL_DestroyTexture()
 */
SDL_Texture *SDL_ILBM_createTexture(SDL_Renderer *renderer, Uint32 format, int access, const SDL_ILBM_Display *display);

/**
 * Blits an image to an SDL texture so that it can be rendered to a window.
 *
 * @param display An SDL_ILBM_Display instance
 * @param pixels Point to the pixel surface area
 * @param pitch The size of each scanline in bytes
 * @return TRUE in case success, else FALSE
 */
int SDL_ILBM_blitDisplayToTexture(SDL_ILBM_Display *display, Uint32 format, void *pixels, int pitch);

/**
 * Renders the texture to a window while taking the window's dimensions and the
 * viewer's offset into account clipping where necessary. This function is
 * basically a wrapper around SDL's SDL_RenderCopy()
 *
 * @param renderer An SDL_Renderer instance
 * @param texture SDL texture to render
 * @param x The x offset of the viewer
 * @param y The y offset of the viewer
 * @param display An SDL_ILBM_Display instance
 * @return 0 in case of success, or a value below zero if an error occurs
 */
int SDL_ILBM_renderCopy(SDL_Renderer *renderer, SDL_Texture *texture, int x, int y, const SDL_ILBM_Display *display);

#ifdef __cplusplus
}
#endif

#endif
