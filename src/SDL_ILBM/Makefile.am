lib_LTLIBRARIES = libSDL_ILBM.la
pkginclude_HEADERS = cycle.h image.h window.h colors.h

libSDL_ILBM_la_SOURCES = cycle.c image.c window.c colors.c
libSDL_ILBM_la_CFLAGS = $(LIBIFF_CFLAGS) $(LIBILBM_CFLAGS) $(LIBAMIVIDEO_CFLAGS) $(SDL_CFLAGS)
libSDL_ILBM_la_LIBADD = -lm $(LIBIFF_LIBS) $(LIBILBM_LIBS) $(LIBAMIVIDEO_LIBS) $(SDL_LIBS)
