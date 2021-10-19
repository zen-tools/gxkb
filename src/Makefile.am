## Copyright (C) 2006 Free Software Foundation, Inc.
## This Makefile.am is free software; the Free Software Foundation
## gives unlimited permission to copy, distribute and modify it.

AM_LDFLAGS = -Wl,--as-needed

bin_PROGRAMS = gxkb
gxkb_SOURCES = gxkb.c                           \
               xkb-callbacks.c xkb-callbacks.h  \
               xkb-config.c xkb-config.h        \
               statusicon.c statusicon.h        \
               xkb-util.c xkb-util.h gettext.h
gxkb_CFLAGS = -O2 @GTK_CFLAGS@ @GLIB_CFLAGS@ @LIBXKLAVIER_CFLAGS@ @LIBWNCK_CFLAGS@ @APPINDICATOR_CFLAGS@ -DFLAGSDIR=\"$(datadir)/gxkb/flags\" -DAPPICON=\"$(datadir)/gxkb/logo.png\"
gxkb_LDADD = @GTK_LIBS@ @GLIB_LIBS@ @LIBXKLAVIER_LIBS@ @LIBWNCK_LIBS@ @APPINDICATOR_LIBS@

DISTCLEANFILES = *.bak *.log *~ .deps/*.P
