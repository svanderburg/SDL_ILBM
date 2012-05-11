AC_PREREQ(2.61)
AC_INIT([SDL_ILBM], m4_esyscmd([echo -n $(cat ./version)$VERSION_SUFFIX]))
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([1.7.2 -Wall -Werror])

# Checks for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AM_PROG_CC_C_O
PKG_PROG_PKG_CONFIG

# Checks for libiff library
LIBIFF_REQUIRED=0.1
PKG_CHECK_MODULES(LIBIFF, libiff >= $LIBIFF_REQUIRED)
AC_SUBST(LIBIFF_CFLAGS)
AC_SUBST(LIBIFF_LIBS)

# Checks for libilbm library
LIBILBM_REQUIRED=0.1
PKG_CHECK_MODULES(LIBILBM, libilbm >= $LIBILBM_REQUIRED)
AC_SUBST(LIBILBM_CFLAGS)
AC_SUBST(LIBILBM_LIBS)

# Checks for libamivideo library
LIBAMIVIDEO_REQUIRED=0.1
PKG_CHECK_MODULES(LIBAMIVIDEO, libamivideo >= $LIBAMIVIDEO_REQUIRED)
AC_SUBST(LIBAMIVIDEO_CFLAGS)
AC_SUBST(LIBAMIVIDEO_LIBS)

# Checks for SDL library
AC_CHECK_PROG([SDL_CONFIG], [sdl-config], [sdl-config])

if test -n "$SDL_CONFIG"; then
    if test -z "$SDL_CFLAGS"; then
        SDL_CFLAGS=`$SDL_CONFIG --cflags`
    fi
    
    if test -z "$SDL_LIBS"; then
        SDL_LIBS=`$SDL_CONFIG --libs`
    fi
fi

AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)

# Output
AC_CONFIG_FILES([
Makefile
src/Makefile
src/SDL_ILBM.pc
src/SDL_ILBM/Makefile
src/ilbmviewer/Makefile
doc/Makefile
doc/man/Makefile
])
AC_OUTPUT
