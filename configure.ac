AC_PREREQ([2.61])
AC_INIT([unieject], [6], [flameeyes@gmail.com])
AC_SUBST(PACKAGE_TARNAME)

AM_INIT_AUTOMAKE([1.10])
AM_CONFIG_HEADER([config.h])

AC_PROG_CC_C99
if test "x$ac_cv_prog_cc_c99" = "xno"; then
   AC_MSG_ERROR([no C99 compiler found, unieject requires a C99 compiler.])
fi

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
	dragonfly*)	libcdiomin="0.77" ;;
	freebsd*)	libcdiomin="0.76" ;;
	*)		libcdiomin="0.74" ;;
esac

PKG_PROG_PKG_CONFIG
AC_SUBST(libcdiomin)
PKG_CHECK_MODULES([LIBCDIO], [libcdio >= $libcdiomin])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.6])

AC_ARG_WITH([libconfuse], [AS_HELP_STRING([--without-libconfuse],
	[Don't use libconfuse for configuration file])])

if test "x$with_libconfuse" != "xno"; then
	PKG_CHECK_MODULES([CONFUSE], [libconfuse >= 2.5])
	AC_DEFINE([HAVE_LIBCONFUSE], 1, [Define if building with libconfuse])
fi

dnl Mandatory functions and headers
AC_CHECK_HEADERS([stdlib.h unistd.h stdio.h string.h unistd.h])
AC_CHECK_FUNCS([perror strerror readlink dirname strdup],, [
	AC_MSG_FAILURE([Unable to find needed functions.])
	])

dnl This is needed for libcdio to work as intended
AC_CHECK_HEADERS([sys/types.h])

dnl Optional functions
AC_CHECK_HEADERS([libgen.h])
AC_CHECK_FUNCS([getenv umount2 setlocale])

AC_ARG_ENABLE([lock-workaround],
	AS_HELP_STRING([--enable-lock-workaround], [Enable Linux CD-Rom lock workaround]))

case $host_os in
	linux* ) os="Linux"
		if test "x$enable_lock_workaround" != "xno"; then
			AC_CHECK_HEADERS([linux/cdrom.h fcntl.h sys/ioctl.h errno.h], ,
				AC_MSG_FAILURE([Unable to find needed headers]))
			AC_DEFINE([USE_LOCK_WORKAROUND], 1, [Use Linucd CD-Rom lock workaround])
		fi
		;;
	dnl DragonFly BSD uses the same interface as FreeBSD, luckily
	freebsd*|dragonfly* ) os="FreeBSD"
		AC_DEFINE([FREEBSD_DRIVER], [1], [Enable FreeBSD driver])
		;;
	
	* ) os="Unsupported"
		AC_MSG_ERROR([Unsupported Operating System detected, please report back.])
		;;
esac

AM_CONDITIONAL([LINUX], [test "x$os" = "xLinux"])
AM_CONDITIONAL([FREEBSD], [test "x$os" = "xFreeBSD"])

CC_ATTRIBUTE_CONSTRUCTOR(, [AC_MSG_FAILURE([Constructor attribute is needed to build unieject.])] )
CC_ATTRIBUTE_FORMAT
CC_ATTRIBUTE_NONNULL
CC_FUNC_EXPECT

CC_ATTRIBUTE_VISIBILITY
CC_FLAG_VISIBILITY([CFLAGS="$CFLAGS -fvisibility=hidden"])

dnl Checking for support programs
GNU_PROG_SED([], [changelog=no])
AC_SUBST([GSED])

dnl Make sure pre-requisites for ChangeLog regeneration are satisfied
AM_CONDITIONAL([REGEN_CHANGELOG], [test "x$changelog" != "xno"])

DOXYGEN_DOC([Doxyfile])

AC_ARG_ENABLE([experimental-ldflags],
	AS_HELP_STRING([--enable-experimental-ldflags], [Enable support for extra LDFLAGS during linking (GNU ld only)]))

if test "x$enable_experimental_ldflags" = "xyes"; then
	EXP_LDFLAGS="-Wl,--as-needed"
fi

ac_save_LDFLAGS=$LDFLAGS
LDFLAGS="$LDFLAGS -Wl,-z,defs"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[]], [[]])],
	       [EXP_LDFLAGS="$EXP_LDFLAGS -Wl,-z,-defs"], [])
LDFLAGS=$ac_save_LDFLAGS

AC_ARG_ENABLE([maintainer-flags],
	AS_HELP_STRING([--enable-maintainer-flags], [Enable extra CFLAGS for maintainers (don't use as user)]))

if test "x$enable_maintainer_flags" = "xyes"; then
	CFLAGS="$CFLAGS -Wall -Wextra -pedantic -Werror -fno-common -Wstrict-aliasing=2"
fi

AC_SUBST(EXP_LDFLAGS)

AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([Makefile
	manpages/Makefile
	m4/Makefile
	lib/libunieject.pc])

AC_OUTPUT
