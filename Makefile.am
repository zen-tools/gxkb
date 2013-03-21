## Copyright (C) 2006 Free Software Foundation, Inc.
## This Makefile.am is free software; the Free Software Foundation
## gives unlimited permission to copy, distribute and modify it.

SUBDIRS = src man doc data
EXTRA_DIST = autogen.sh debian/patches debian/watch \
             debian/source/format debian/changelog \
             debian/compat debian/control \
             debian/copyright debian/rules

# Don't put .svn directories in the distribution file
dist-hook:
	rm -rf `find $(distdir)/ -type d -name .svn`
