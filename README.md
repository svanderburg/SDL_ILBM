SDL_ILBM
========
SDL_ILBM is a package containing a library for displaying ILBM images and to
manage their state in SDL applications. Additionally, this package contains
`ilbmviewer`, a command-line utility which can be used to view a collection ILBM
images inside an IFF file. Furthermore, it supports some of ILBM's advanced
(and less commonly used) features, such as color range cycling.

Prerequisites
=============
In order to build and use this package the following libraries are required:

* `libiff`, in order to parse IFF files
* `libilbm`, in order to understand ILBM application chunks, byte-run
  compression/decompression and to conveniently access ILBM image properties
* `libamivideo`, in order to convert Amiga planar graphics data into RGB and to
  display special screen modes such as EHB and HAM
* [SDL](http://www.libsdl.org), is used as a portability layer to handle input
  and graphics

Installation on Unix-like systems
=================================
Compilation and installation of this library on Unix-like systems is straight
forward, by using the standard GNU autotools build instructions:

    $ ./configure
    $ make
    $ make install

More details about the installation process can be found in the `INSTALL` file
included in this package.

Building with Visual C++
========================
This package can also be built with Visual C++ for Windows platforms. The
solution file resides in `src/SDL_ILBM.sln` that can be opened in Visual Studio
to edit or build it. Alternatively, you can also use `MSBuild` to compile it:

    $ MSBuild SDL_ILBM.sln

To make any builds work, you must have a build for `libiff`, `libilbm`,
`libamivideo` and `SDL 2.0` first. By default, the project file looks for these
folders in the parent directory of the current solution.

You can also specify their locations through property parameters. For example,
the following instructions specifies where `libiff` includes and libraries can
be found:

    $ MSBuild /p:libiffIncludePath:..\..\..\libiff\src /p:libiffLibPath:..\..\..\libiff\src\Debug libilbm.sln

The following property parameters can be used:

* `libiffIncludePath`, `libiffLibPath`
* `libilbmIncludePath`, `libilbmLibPath`
* `libamiVideoIncludePath`, `libamivideoLibPath`
* `SDL2IncludePath`, `SDL2LibPath`

The output is produced in the `Debug/` directory.

Using the SDL_ILBM API
======================
The `SDL_ILBM` package provides an API that can be used to read IFF files
containing ILBM images and allows easy usage of these images in SDL
applications, including some ILBM's advanced features, such as color cycling.
Full API documentation can be found in the `doc/apidox` directory of this
package.

Generating an image set
-----------------------
An ILBM file typically provides one single FORM representing an image, but could
also encapsulate multiple images and other types of forms, in so called "IFF
scrap files". The `SDL_ILBM_Set` struct provides an interface to access all ILBM
images (and its variants, such as ACBM and PBM images) in a particular IFF file:

```C
#include <set.h>

SDL_ILBM_Set *set = SDL_ILBM_createSetFromFilename("somefile.IFF");

if(set != NULL && set->imagesLength > 0)
{
    /* Open an image */
    
    return 0;
}
else
    return 1; /* Cannot find any images to display */
```

The above code fragment generates an image set directly from a file with a given
filename. In addition to generating sets from file names, it is also possible
to generate a set from a file descriptor (through: `SDL_ILBM_createSetFromFd()`)
or an `IFF_Chunk` (through: `SDL_ILBM_createSetFromIFFChunk()`).

When the work is done, a set must be deallocated from heap memory, through:

```C
SDL_ILBM_freeSet(set);
```

Displaying a still image
------------------------
The simplest use case of the `SDL_ILBM` API is retrieving an ILBM image from the
`SDL_ILBM_Set` and displaying it as a still image, by generating an
`SDL_Surface` and blitting it to another surface or texture.

The following instruction generates an `SDL_Surface` for the first ILBM image in
the image set:

```C
#include <set.h>

SDL_Surface *pictureSurface = SDL_ILBM_createSurfaceFromSet(set, 0, 1, SDL_ILBM_RGB_FORMAT);
```

The above invocation refers to the image in the set with index `0`, uses `1` as
lowres pixel scale factor (the width of each source pixel in destination pixels)
and "RGB" as output format (meaning that the corresponding SDL surface uses 32-bit
true color graphics).

The `SDL_ILBM` API allows you to pick various options for the lowres pixel scale
factor and the format. See sections: 'Choosing a lowres pixel scale factor' and
'Choosing an output format' for more information.

The above `SDL_Surface` instance can be blitted to another surface as follows:

```C
#include <SDL.h>

if(pictureSurface != NULL)
{
    SDL_BlitSurface(pictureSurface, NULL, screenSurface, NULL);
    SDL_FreeSurface(pictureSurface);
}
```

Consult SDL's documentation for more details on how to work with `SDL_Surface`s
for displaying 2D graphics.

Displaying an animatable/cyclable image
---------------------------------------
Besides displaying still images, it may also be desired to animate an image by
using its color cycling properties. The image module encapsulates relevant
functionality to do this conveniently.

The following command line instruction composes an `SDL_ILBM_Image` instance for
the first image in an `SDL_ILBM_Set`, using the same display settings as in the
previous example:

```C
#include <image.h>

SDL_ILBM_Image *image = SDL_ILBM_createImageFromSet(set, 0, 1, SDL_ILBM_RGB_FORMAT);
```

Blitting an `SDL_ILBM_Image` instance to an `SDL_Surface` can be done as follows:

```C
SDL_ILBM_blitImageToSurface(image, NULL, destSurface, NULL);
```

The above function invocation blits the entire image to the given destination
surface.

The `SDL_ILBM_blitImageToSurface()` function is basically a wrapper around SDL's
`SDL_BlitSurface()` function and returns the same exit values (0 in case of
success or a value below zero in case of an error). It can also blit from and to
sub areas of surfaces by defining the `srcrect` and `dstrect` parameters (which
have been set to `NULL` in the example).

Cycling the colors of an animatable/cyclable image
--------------------------------------------------
We can shift the color palette of an image (according to the color range
specifications in the image) by running:

```C
SDL_ILBM_cycleColors(image);
```

and restoring the palette back to normal, with:

```C
SDL_ILBM_resetColors(image);
```

Each time the palette changes, we must reblit/redraw the surface (e.g. through
`SDL_ILBM_blitImageToSurface()`) to make the color cycling changes visible.

To fully animate a cyclable image, we must regularly reinvoke these functions
(for smooth playback: it should be done at least 50 times per second). For
example, it can be done by integrating these invocations into the main loop or
by invoking them in a timer.

The `SDL_ILBM_Image` struct internally maintains a table of range times to
determine which colors should be changed at a certain time, so there is no need
to worry about this. The only obligation is that the `SDL_ILBM_cycleColors()`
function (and corresponding redraw function) should be invoked regularly.

Creating a display window for images
------------------------------------
We may also want to construct a window that has the appropriate dimensions for
displaying an image. This library provides a display module that can be used to
perform some of the required "house keeping" tasks.

The following instruction constructs a display that has the same dimensions as
an image's page width and height:

```C
#include <display.h>

SDL_ILBM_Display *display = SDL_ILBM_createDisplay(image, FALSE /* the window's dimensions are equal to the page dimensions */);
```

The page width and page height correspond to the physical dimensions of an Amiga
display. Sometimes the logical width and height of an image could be bigger
than the dimensions of the physical screen. In such cases, the Amiga display
clips the image and the screen offsets need to be adjusted to display other sub
areas of the image.

Modern displays have much bigger display resolutions and can easily display
such images entirely on one screen. By setting the stretch parameter to `TRUE`,
a window will be constructed whose dimensions are equal to the logical sizes of
the image:

```C
SDL_ILBM_Display *display = SDL_ILBM_createDisplay(image, TRUE /* stretch the window to match an image's logical dimensions */);
```

We can also use the display module to conveniently construct an SDL window whose
dimensions are identical to the display:

```C
SDL_Window *window = SDL_ILBM_createWindow("My Cool ILBM Image", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, display, 0)
```

The above function is basically a wrapper around SDL's `SDL_CreateWindow()`
function and takes similar parameters. Furthermore, it has the same return
values.

In addition to a window, an SDL application also typically needs a renderer
which purpose is to render graphics to the window:

```C
#include <SDL.h>

SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
SDL_RenderClear(renderer);
```

As with the SDL window, the renderer's dimensions typically need to correspond
to the display's dimensions as well. We can use the following wrapper function
to accomplish this:

```C
SDL_ILBM_renderSetLogicalSize(renderer, display);
```

The above function is a wrapper around SDL's `SDL_RenderSetLogicalSize()`
function taking similar parameters and returning the same exit values.

To render graphics to the window, we must allocate a texture in hardware memory.
As with the window and renderer, the texture's dimensions also typically need to
correspond to the display. The following command creates a streaming texture with
the same dimensions as the display:

```C
SDL_Texture *texture = SDL_ILBM_createTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, display);
```

The above function is a wrapper around SDL's `SDL_CreateTexture()` function,
takes similar parameters, and returns the same exit values.

The above instructions are based on
[SDL's 1.2 to 2.0 migration guide](https://wiki.libsdl.org/MigrationGuide)
instructions. For more detailed information on these function calls, consult the
SDL manual.

Blitting an ILBM image to a texture
-----------------------------------
In addition to `SDL_Surface`s, we may also want to blit an image to a texture so
that we can render an image to a window. The above code snippet illustrates how
to do this with the streaming texture that we have constructed previously:

```C
void *pixels;
int pitch;

if(SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0)
    fprintf(stderr, "Cannot lock texture: %s\n", SDL_GetError());

if(!SDL_ILBM_blitDisplayToTexture(display, SDL_PIXELFORMAT_RGBA8888, pixels, pitch))
    fprintf(stderr, "Cannot blit image to texture!\n");

SDL_UnlockTexture(texture);
```

Rendering a texture to a window
-------------------------------
In addition to rendering an image to a texture, we must also render a texture to
a window so that its changes become visible. This library offers a convenience
function that renders a texture to a display, while taking a display offset
into account and clipping the image where necessary:

```C
SDL_ILBM_renderCopy(renderer, texture, 20 /* x offset */, 20 /* y offset */, display);
```

The function shown above is a wrapper around SDL's `SDL_RenderCopy()` function
taking similar pararmeters and returning the same exit values.

After rendering a texture, we must always call the following function to swap
the display buffers so that the rendered graphics become visible:

```C
SDL_RenderPresent(renderer);
```

Choosing a lowres pixel scale factor
====================================
On PCs, resolutions refer to the amount of pixels per scanline and the amount of
scanlines per screen. On the Amiga, resolutions only refer to the amount of
pixels per scanline and only a few fixed resolutions can be used.

For example, a high resolution screen has twice the amount of pixels per
scanline compared to a low resolution screen. A super hires screen has double
the amount of pixels per scanline compared to a high resolution screen.
Moreover, a low resolution pixel is twice a wide as a high resolution pixel and
so on.

Vertically, there are only two options. In principle, there are a fixed amount
of scanlines on a display. The amount of scanlines can be doubled, using a
so-called interlace screen mode. However, interlace screen modes have a big
drawback -- they draw the odd scanlines in one frame and the even ones in
another. On displays with a short after glow, flickering may be observed.

If we would convert an Amiga image directly to a PC display, we may observe odd
results in some cases. For example, a non-interlaced high resolution image looks
twice as wide on a PC display than on an Amiga display. To give it the same
look, we must correct its aspect ratio by doubling the amount of scanlines on
the PC display.

* By picking `1` as a lowres pixel scale factor, each pixel in the bitplane
  surface translates to a pixel in the converted chunky/RGB surface, breaking
  the aspect ratio for certain kinds of images.

* By picking `2`, we can retain the aspect ratio for lowres and hires images.
  For example, a lowres, non-interlaced picture is twice as wide and twice as
  high as the original image. For a high resolution, non-interlaced image, the
  amount of scanlines are doubled.
  
* By picking `4`, we can retain the aspect ratio for super hires images in
  addition to hires and lowres images. Lowres, non-interlaced images are four
  time as wide and four times a high as the original.

* By picking `0`, the API attempts to find the smallest lowres pixel scale
  factor that retains the aspect ratio.

Choosing an output format
=========================
We can convert planar graphics surfaces to surfaces with the following output
formats:

* `SDL_ILBM_CHUNKY_FORMAT` composes a chunky surface having a 256-color palette in
  which each byte represents a pixel referring to a color in the palette.
  A limitation of this output format is that it cannot display images using the
  HAM screen mode or images with 24 or 32 bitplanes (essentially true-color
  images).

* `SDL_ILBM_RGB_FORMAT` composes an RGB surface in which each pixel is encoded
  as 4 bytes representing the red color, green and blue color intensity. The
  remaining byte is used as an alpha layer for transparency. It can display all
  possible images, but is more memory consuming.

* `SDL_ILBM_AUTO_FORMAT` chooses the most memory efficient option -- it will
  choose `SDL_ILBM_CHUNKY_FORMAT` unless the bitplane surface requires more than
  256 colors. If the latter is the case, then it will choose
  `SDL_ILBM_AUTO_FORMAT`.

ILBM viewer command-line utility
================================
This package also includes a command-line utility `ilbmviewer` that exposes most
of the API's features as command-line options. For more information, run:

    $ ilbmviewer --help

License
=======
This library is available under the zlib license
