AC_PREREQ(2.62)
AC_INIT([unieject], [6], [flameeyes@gmail.com])
AC_SUBST(PACKAGE_TARNAME)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([config])

AM_INIT_AUTOMAKE([1.10])
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

AC_PROG_CC_C99
AS_IF([test "x$ac_cv_prog_cc_c99" = "xno"], [
  AC_MSG_ERROR([no C99 compiler found, unieject requires a C99 compiler.])
])

AC_USE_SYSTEM_EXTENSIONS

LT_PREREQ([2.2.4])
LT_INIT

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])

AS_CASE([$host_os],
  [dragonfly*], [libcdiomin="0.77"],
  [freebsd*],   [libcdiomin="0.76"],
                [libcdiomin="0.74"])

PKG_PROG_PKG_CONFIG
AC_SUBST(libcdiomin)
PKG_CHECK_MODULES([LIBCDIO], [libcdio >= $libcdiomin])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.6])

PKG_WITH_MODULES([CONFUSE], [libconfuse >= 2.5],
  [AC_DEFINE([HAVE_LIBCONFUSE], 1, [Define if building with libconfuse])],
  [],
  [Use libconfuse for configuration file parsing], [yes])

dnl Mandatory functions and headers
AC_CHECK_HEADERS([stdlib.h unistd.h stdio.h string.h unistd.h])
AC_CHECK_FUNCS([perror strerror readlink dirname strdup],, [
	AC_MSG_FAILURE([Unable to find needed functions.])
	])

dnl This is needed for libcdio to work as intended
dnl Remove once cdio will stop checking for HAVE_SYS_TYPES_H
AC_CHECK_HEADERS([sys/types.h])

dnl Optional functions
AC_CHECK_HEADERS([libgen.h])
AC_CHECK_FUNCS([getenv umount2 setlocale])

AC_ARG_ENABLE([lock-workaround],
	AS_HELP_STRING([--enable-lock-workaround], [Enable Linux CD-Rom lock workaround]))

AS_CASE([$host_os],
  [linux*], [
    os="Linux"
    AS_IF([test "x$enable_lock_workaround" != "xno"], [
      AC_CHECK_HEADERS([linux/cdrom.h fcntl.h sys/ioctl.h errno.h], ,
        AC_MSG_FAILURE([Unable to find needed headers]))
      AC_DEFINE([USE_LOCK_WORKAROUND], 1, [Use Linucd CD-Rom lock workaround])
    ])
  ],
  dnl DragonFly BSD uses the same interface as FreeBSD, luckily
  [freebsd*|dragonfly*], [
    os="FreeBSD"
    AC_DEFINE([FREEBSD_DRIVER], [1], [Enable FreeBSD driver])
  ],
  [os="Unsupported"
   AC_MSG_ERROR([Unsupported Operating System detected, please report back.])
  ])

AM_CONDITIONAL([LINUX], [test "x$os" = "xLinux"])
AM_CONDITIONAL([FREEBSD], [test "x$os" = "xFreeBSD"])

CC_ATTRIBUTE_CONSTRUCTOR(, [AC_MSG_FAILURE([Constructor attribute is needed to build unieject.])] )
CC_ATTRIBUTE_FORMAT
CC_ATTRIBUTE_NONNULL
CC_FUNC_EXPECT

CC_ATTRIBUTE_VISIBILITY([protected])
CC_ATTRIBUTE_VISIBILITY([hidden])
CC_ATTRIBUTE_VISIBILITY([internal])
CC_FLAG_VISIBILITY([CFLAGS="$CFLAGS -fvisibility=hidden"])

dnl Checking for support programs
AC_CHECK_PROG([changelog], [git], [yes])

dnl Make sure pre-requisites for ChangeLog regeneration are satisfied
AM_CONDITIONAL([REGEN_CHANGELOG], [test "x$changelog" = "xyes"])

DOXYGEN_DOC([Doxyfile])

AC_ARG_ENABLE([experimental-ldflags],
	AS_HELP_STRING([--enable-experimental-ldflags], [Enable support for extra LDFLAGS during linking (GNU ld only)]))

AS_IF([test "x$enable_experimental_ldflags" = "xyes"], [
  CC_CHECK_LDFLAGS([-Wl,--as-needed], [EXP_LDFLAGS="-Wl,--as-needed"])
])

CC_CHECK_LDFLAGS([-Wl,-z,defs], [EXP_LDFLAGS="$EXP_LDFLAGS -Wl,-z,defs"])

AC_ARG_ENABLE([maintainer-flags],
	AS_HELP_STRING([--enable-maintainer-flags], [Enable extra CFLAGS for maintainers (don't use as user)]))

AS_IF([test "x$enable_maintainer_flags" = "xyes"], [
  CFLAGS="$CFLAGS -Wall -Wextra -pedantic -Werror -fno-common -Wstrict-aliasing=2"
])

AC_SUBST(EXP_LDFLAGS)

AC_CONFIG_FILES([po/Makefile.in])
AC_CONFIG_FILES([Makefile
	manpages/Makefile
	m4/Makefile
	lib/libunieject.pc])

AC_OUTPUT
