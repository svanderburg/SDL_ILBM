bin_PROGRAMS = ilbmviewer
noinst_HEADERS = viewer.h

ilbmviewer_SOURCES = main.c viewer.c
ilbmviewer_LDADD = ../SDL_ILBM/libSDL_ILBM.la $(LIBIFF_LIBS) $(LIBILBM_LIBS) $(LIBAMIVIDEO_LIBS) $(SDL2_LIBS)
ilbmviewer_CFLAGS = -I../SDL_ILBM $(LIBIFF_CFLAGS) $(LIBILBM_CFLAGS) $(LIBAMIVIDEO_CFLAGS) $(SDL2_CFLAGS)
