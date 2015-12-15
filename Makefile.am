## Copyright (C) 2006 Free Software Foundation, Inc.
## This Makefile.am is free software; the Free Software Foundation
## gives unlimited permission to copy, distribute and modify it.

SUBDIRS = images
EXTRA_DIST = debian COPYING CREDITS INSTALL NEWS TODO

bin_PROGRAMS = gxkb
gxkb_SOURCES = gxkb.c xkb-callbacks.c xkb-config.c xkb-util.c common.h xkb-callbacks.h xkb-config.h xkb-util.h
gxkb_CFLAGS = -O2 @GTK_CFLAGS@ @GLIB_CFLAGS@ @LIBXKLAVIER_CFLAGS@ @LIBWNCK_CFLAGS@ -DFLAGSDIR=\"$(datadir)/gxkb/flags\"
gxkb_LDADD = @GTK_LIBS@ @GLIB_LIBS@ @LIBXKLAVIER_LIBS@ @LIBWNCK_LIBS@

# Don't put .svn directories in the distribution file
dist-hook:
	rm -rf `find $(distdir)/ -type d -name .svn`