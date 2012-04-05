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

Installation
============
Compilation and installation of this library is straight forward, by using the
standard GNU autotools build instructions:

    $ ./configure
    $ make
    $ make install

More details about the installation process can be found in the `INSTALL` file
included in this package.

License
=======
This library is available under the zlib license
