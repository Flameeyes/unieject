AC_PREREQ(2.59)
AC_INIT([unieject], [5.1], [flameeyes@gentoo.org])
AC_SUBST(PACKAGE_TARNAME)

AM_INIT_AUTOMAKE([unieject], [5.1])
AM_CONFIG_HEADER([config.h])

AC_GNU_SOURCE

dnl Next four lines is a hack to prevent libtool checking for CXX/F77
m4_undefine([AC_PROG_CXX])
m4_defun([AC_PROG_CXX],[])
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])
AC_PROG_LIBTOOL

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.14])

case $host_os in
	freebsd*)	libcdiomin="0.76" ;;
	*)		libcdiomin="0.74" ;;
esac

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([LIBCDIO], [libcdio >= $libcdiomin])
AC_SUBST(LIBCDIO_CFLAGS)
AC_SUBST(LIBCDIO_LDFLAGS)
AC_SUBST(LIBCDIO_LIBS)

AC_ARG_WITH([libconfuse], [AS_HELP_STRING([--without-libconfuse],
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

AC_ARG_ENABLE([lock-workaround],
	AS_HELP_STRING([--enable-lock-woraround], [Enable Linux CD-Rom lock workaround]))

case $host_os in
	linux* ) os="Linux"
		if test "x$enable_lock-workaround" != "xno"; then
			AC_CHECK_HEADERS([linux/cdrom.h fcntl.h sys/ioctl.h errno.h], ,
				AC_MSG_FAILURE([Unable to find needed headers]))
			AC_DEFINE([USE_LOCK_WORKAROUND], 1, [Use Linucd CD-Rom lock workaround])
		fi
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
CC_FUNC_EXPECT

dnl Checking for support programs
GNU_PROG_SED(, [AC_MSG_FAILURE([GNU sed is needed to build unieject.])])
AC_SUBST([GSED])

LANG_C99(, [AC_MSG_FAILURE([C99 support is required to build unieject.])])

DOXYGEN_DOC([Doxyfile])

AC_ARG_ENABLE([experimental-ldflags],
	AS_HELP_STRING([--enable-experimental-ldflags], [Enable support for extra LDFLAGS during linking (GNU ld only)]))

if test "x$enable_experimental_ldflags" = "xyes"; then
	EXP_LDFLAGS="-Wl,--as-needed -Wl,--not-add-needed"
fi

AC_SUBST(EXP_LDFLAGS)

AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([Makefile
	lib/Makefile
	manpages/Makefile
	m4/Makefile
	lib/libunieject.pc])

AC_OUTPUT
