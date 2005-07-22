AC_PREREQ(2.59)
AC_INIT([unieject], [0], [flameeyes@gentoo.org])

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])
AC_PROG_LIBTOOL

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([LIBCDIO], [libcdio >= 0.74])
AC_SUBST(LIBCDIO_CFLAGS)
AC_SUBST(LIBCDIO_LDFLAGS)

AM_PATH_LIBPOPT
AC_SUBST(LIBPOPT_CFLAGS)
AC_SUBST(LIBPOPT_LDFLAGS)

dnl Mandatory functions and headers
AC_CHECK_HEADERS([stdlib.h unistd.h stdio.h string.h unistd.h])
AC_CHECK_FUNCS([perror])

dnl Optional functions
AC_CHECK_HEADERS([libgen.h])
AC_CHECK_FUNCS([getenv readlink dirname])

AC_OUTPUT([Makefile
	lib/Makefile])
