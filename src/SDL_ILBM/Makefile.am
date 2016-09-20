lib_LTLIBRARIES = libSDL_ILBM.la
pkginclude_HEADERS = set.h cycle.h image.h display.h image2amivideo.h amivideo2surface.h render.h

libSDL_ILBM_la_SOURCES = set.c cycle.c image.c display.c image2amivideo.c amivideo2surface.c render.c
libSDL_ILBM_la_CFLAGS = $(LIBIFF_CFLAGS) $(LIBILBM_CFLAGS) $(LIBAMIVIDEO_CFLAGS) $(SDL2_CFLAGS)
libSDL_ILBM_la_LIBADD = -lm $(LIBIFF_LIBS) $(LIBILBM_LIBS) $(LIBAMIVIDEO_LIBS) $(SDL2_LIBS)
