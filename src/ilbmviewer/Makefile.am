ilbmviewer.1: main.c
	$(HELP2MAN) --output=$@ --no-info --name 'View a collection of ILBM images inside an IFF file' --include=ilbmviewer.h2m --libtool ./ilbmviewer

bin_PROGRAMS = ilbmviewer
noinst_HEADERS = viewer.h
man1_MANS = ilbmviewer.1

ilbmviewer_SOURCES = main.c viewer.c
ilbmviewer_LDADD = ../SDL_ILBM/libSDL_ILBM.la $(LIBIFF_LIBS) $(LIBILBM_LIBS) $(LIBAMIVIDEO_LIBS) $(SDL2_LIBS)
ilbmviewer_CFLAGS = -I../SDL_ILBM $(LIBIFF_CFLAGS) $(LIBILBM_CFLAGS) $(LIBAMIVIDEO_CFLAGS) $(SDL2_CFLAGS)

EXTRA_DIST = ilbmviewer.1 ilbmviewer.h2m
