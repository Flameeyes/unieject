AC_PREREQ(2.59)
AC_INIT([unieject], [4], [flameeyes@gentoo.org])
AC_SUBST(PACKAGE_TARNAME)

AM_INIT_AUTOMAKE([unieject], [4])
AM_CONFIG_HEADER([config.h])

AC_GNU_SOURCE
AC_PROG_LIBTOOL
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.14])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([LIBCDIO], [libcdio >= 0.74])
AC_SUBST(LIBCDIO_CFLAGS)
AC_SUBST(LIBCDIO_LDFLAGS)
AC_SUBST(LIBCDIO_LIBS)

AC_ARG_WITH([libconfuse], [AC_HELP_STRING([--without-libconfuse],
	[Don't use libconfuse for configuration file])])

if test "x$with_libconfuse" != "xno"; then
	PKG_CHECK_MODULES([CONFUSE], [libconfuse >= 2.5])
	AC_DEFINE([HAVE_LIBCONFUSE], 1, [Define if building with libconfuse])
fi
AC_SUBST(CONFUSE_CFLAGS)
AC_SUBST(CONFUSE_LDFLAGS)
AC_SUBST(CONFUSE_LIBS)

AM_PATH_LIBPOPT
AC_SUBST(LIBPOPT_CFLAGS)
AC_SUBST(LIBPOPT_LDFLAGS)
AC_SUBST(LIBPOPT_LIBS)

dnl Mandatory functions and headers
AC_CHECK_HEADERS([stdlib.h unistd.h stdio.h string.h unistd.h])
AC_CHECK_FUNCS([perror strerror readlink dirname strdup],, [
	AC_MSG_FAILURE([Unable to find needed functions.])
	])

dnl Optional functions
AC_CHECK_HEADERS([libgen.h])
AC_CHECK_FUNCS([getenv umount2])

case $host_os in
	linux* ) os="Linux"
		;;
	freebsd* ) os="FreeBSD"
		;;
	
	* ) os="Unsupported"
		;;
esac

AM_CONDITIONAL([LINUX], [test "x$os" = "xLinux"])
AM_CONDITIONAL([FREEBSD], [test "x$os" = "xFreeBSD"])

CC_ATTRIBUTE_CONSTRUCTOR(, [AC_MSG_FAILURE([Constructor attribute is needed to build unieject.])] )
CC_ATTRIBUTE_FORMAT
CC_ATTRIBUTE_INTERNAL
CC_ATTRIBUTE_NONNULL

AC_MSG_CHECKING([if compiler supports -std=c99 flag])
AC_COMPILE_IFELSE([int a;], [
		AC_MSG_RESULT([yes])
		CFLAGS="${CFLAGS} -std=c99"
	], [
		AC_MSG_RESULT([no])
	])

dnl Checking for support programs
GNU_PROG_SED(, [AC_MSG_FAILURE([GNU sed is needed to build unieject.])])
AC_SUBST([GSED])

PROG_TXT2MAN(, [AC_MSG_WARN([txt2man was not found or was disabled, man pages won't be built])])
AM_CONDITIONAL([HAVE_TXT2MAN], [test "x$TXT2MAN" != "xno"])

DOXYGEN_DOC([Doxyfile])

AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([Makefile
	lib/Makefile
	manpages/Makefile
	m4/Makefile
	lib/libunieject.pc])

AC_OUTPUT
