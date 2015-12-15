AC_INIT([gxkb], [2.0], [zen@root.ua])
AM_INIT_AUTOMAKE([-Wall -Werror foreign ])
AC_CONFIG_SRCDIR([gxkb.c])
AC_CONFIG_HEADERS([config.h])
AC_PROG_CC
AC_HEADER_STDC
PKG_PROG_PKG_CONFIG([0.20])
PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.24.10])
PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.32.4])
PKG_CHECK_MODULES(LIBXKLAVIER, [libxklavier >= 5.2.1])
PKG_CHECK_MODULES(LIBWNCK, [libwnck-1.0 >= 2.30.7])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
AC_SUBST(LIBXKLAVIER_CFLAGS)
AC_SUBST(LIBXKLAVIER_LIBS)
AC_SUBST(LIBWNCK_CFLAGS)
AC_SUBST(LIBWNCK_LIBS)
AC_DEFINE(WNCK_I_KNOW_THIS_IS_UNSTABLE, 1, [libwnck is happy with this])
AC_DEFINE(HAVE_CONFIG_H,1,[Include config.h])
if test "x$prefix" = "xNONE" ; then
  prefix="/usr/local"
fi
if test "x$exec_prefix" = "xNONE" ; then
  exec_prefix=$prefix
fi
#AC_CONFIG_FILES([
#Makefile
#])
AC_OUTPUT([Makefile])