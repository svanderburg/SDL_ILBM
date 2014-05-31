SDL_ILBM
========
SDL_ILBM is a package containing a library for displaying ILBM images and to
manage their state in SDL applications. Additionally, this package contains
`ilbmviewer`, a command-line utility which can be used to view a collection ILBM
images inside an IFF file and to enable the cycling of color ranges.

Prerequisites
=============
In order to build and use this package the following libraries are required:

* `libiff`, in order to parse IFF files
* `libilbm`, in order to understand ILBM application chunks, byte-run compression/decompression and to conveniently access ILBM image properties
* `libamivideo`, in order to convert Amiga planar graphics data into RGB and to display special screen modes such as EHB and HAM
* [SDL](http://www.libsdl.org), is used as a portability layer to handle input and graphics

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

License
=======
This library is available under the zlib license
