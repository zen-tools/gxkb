## Copyright (C) 2006 Free Software Foundation, Inc.
## This Makefile.am is free software; the Free Software Foundation
## gives unlimited permission to copy, distribute and modify it.

SUBDIRS = src images
EXTRA_DIST = debian COPYING CREDITS INSTALL NEWS TODO

# Don't put .svn directories in the distribution file
dist-hook:
	rm -rf `find $(distdir)/ -type d -name .svn`