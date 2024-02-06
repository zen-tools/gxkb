AC_INIT([gxkb], [0.9.5], [dmytro.poltavchenko@gmail.com], [gxkb], [https://zen-tools.github.io/gxkb])
AM_INIT_AUTOMAKE([-Wall -Werror foreign ])
AC_CONFIG_SRCDIR([src/gxkb.c])
AC_CONFIG_HEADERS([src/config.h])
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_CC_C_O
AC_HEADER_STDC
PKG_PROG_PKG_CONFIG([0.20])
PKG_CHECK_MODULES(GTK, [gtk+-3.0])
PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.16.0])
PKG_CHECK_MODULES(LIBWNCK, [libwnck-3.0])
PKG_CHECK_MODULES(LIBXKLAVIER, [libxklavier >= 5.0])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
AC_SUBST(LIBXKLAVIER_CFLAGS)
AC_SUBST(LIBXKLAVIER_LIBS)
AC_SUBST(LIBWNCK_CFLAGS)
AC_SUBST(LIBWNCK_LIBS)

APPINDICATOR_REQUIRED_VERSION=0.2.4

AC_ARG_ENABLE(
    [appindicator],
    AS_HELP_STRING([--enable-appindicator[=@<:@no/auto/yes@:>@]], [Build support for application indicators]),
    [enable_appindicator=$enableval],
    [enable_appindicator="auto"]
)


if test x$enable_appindicator = xauto ; then
    PKG_CHECK_EXISTS(
        ayatana-appindicator3-0.1 >= $APPINDICATOR_REQUIRED_VERSION,
        [enable_appindicator="yes"],
        [enable_appindicator="no"]
    )
fi

if test x$enable_appindicator = xyes ; then
    PKG_CHECK_MODULES(
        APPINDICATOR,
        [ayatana-appindicator3-0.1 >= $APPINDICATOR_REQUIRED_VERSION],
        [AC_DEFINE(HAVE_APPINDICATOR, 1, [Have AppIndicator])]
    )
fi

AM_CONDITIONAL(HAVE_APPINDICATOR, test x$enable_appindicator = xyes)
AC_SUBST(APPINDICATOR_CFLAGS)
AC_SUBST(APPINDICATOR_LIBS)

AC_DEFINE(AUTHORS, ["Dmytro Poltavchenko <dmytro.poltavchenko@gmail.com>\nMichael Ivanchenko <ivanchenko.michael.work@gmail.com>"], [list of authors])
AC_DEFINE(COPYRIGHT, ["Copyright (C) 2013-2024 Dmytro Poltavchenko"], [copyright])
AC_DEFINE(DESCRIPTION, ["X11 Keyboard switcher"], [description])

AC_DEFINE(WNCK_I_KNOW_THIS_IS_UNSTABLE, 1, [libwnck is happy with this])
AC_DEFINE(HAVE_CONFIG_H, 1, [Include config.h])
AC_PREFIX_DEFAULT(/usr)
AC_OUTPUT([Makefile src/Makefile man/Makefile doc/Makefile data/Makefile data/flags/Makefile])
