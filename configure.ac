AC_INIT([gxkb], [0.7.9], [admin@linuxhub.ru], [gxkb], [https://zen-tools.github.io/gxkb])
AM_INIT_AUTOMAKE([-Wall -Werror foreign ])
AC_CONFIG_SRCDIR([src/gxkb.c])
AC_CONFIG_HEADERS([src/config.h])
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_CC_C_O
AC_HEADER_STDC
PKG_PROG_PKG_CONFIG([0.20])
PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.12.0])
PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.16.0])
PKG_CHECK_MODULES(LIBXKLAVIER, [libxklavier >= 3.0])
PKG_CHECK_MODULES(LIBWNCK, [libwnck-1.0 >= 2.22.0])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
AC_SUBST(LIBXKLAVIER_CFLAGS)
AC_SUBST(LIBXKLAVIER_LIBS)
AC_SUBST(LIBWNCK_CFLAGS)
AC_SUBST(LIBWNCK_LIBS)
LIBXKLAVIER_VERSION=`pkg-config --modversion libxklavier | sed 's/\.//;s/\..*//g'`
AC_DEFINE_UNQUOTED(LIBXKLAVIER_VERSION, ${LIBXKLAVIER_VERSION}, [libxklavier version])

APPINDICATOR_REQUIRED_VERSION=0.2.4

AC_ARG_ENABLE(
    [appindicator],
    AS_HELP_STRING([--enable-appindicator[=@<:@no/auto/yes@:>@]], [Build support for application indicators]),
    [enable_appindicator=$enableval],
    [enable_appindicator="auto"]
)


if test x$enable_appindicator = xauto ; then
    PKG_CHECK_EXISTS(
        appindicator-0.1 >= $APPINDICATOR_REQUIRED_VERSION,
        [enable_appindicator="yes"],
        [enable_appindicator="no"]
    )
fi

if test x$enable_appindicator = xyes ; then
    PKG_CHECK_MODULES(
        APPINDICATOR,
        [appindicator-0.1 >= $APPINDICATOR_REQUIRED_VERSION],
        [AC_DEFINE(HAVE_APPINDICATOR, 1, [Have AppIndicator])]
    )
fi

AM_CONDITIONAL(HAVE_APPINDICATOR, test x$enable_appindicator = xyes)
AC_SUBST(APPINDICATOR_CFLAGS)
AC_SUBST(APPINDICATOR_LIBS)

AC_DEFINE(AUTHORS, ["Dmitriy Poltavchenko <admin@linuxhub.ru>\nMichael Ivanchenko <ivanchenko.michael.work@gmail.com>"], [list of authors])
AC_DEFINE(COPYRIGHT, ["Copyright (C) 2013-2016 Dmitriy Poltavchenko"], [copyright])
AC_DEFINE(DESCRIPTION, ["X11 Keyboard switcher"], [description])

AC_DEFINE(WNCK_I_KNOW_THIS_IS_UNSTABLE, 1, [libwnck is happy with this])
AC_DEFINE(HAVE_CONFIG_H, 1, [Include config.h])
AC_PREFIX_DEFAULT(/usr)
AC_OUTPUT([Makefile src/Makefile man/Makefile doc/Makefile data/Makefile data/flags/Makefile])

