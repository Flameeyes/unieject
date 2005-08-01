AC_PREREQ(2.59)
AC_INIT([unieject], [3], [flameeyes@gentoo.org])
AC_SUBST(PACKAGE_TARNAME)

AC_GNU_SOURCE

AM_INIT_AUTOMAKE([unieject], [3])
AM_CONFIG_HEADER([config.h])
AC_PROG_LIBTOOL

AM_GNU_GETTEXT([external])

PKG_PROG_PKG_CONFIG
PKG_CHECK_MODULES([LIBCDIO], [libcdio >= 0.74])
AC_SUBST(LIBCDIO_CFLAGS)
AC_SUBST(LIBCDIO_LDFLAGS)
AC_SUBST(LIBCDIO_LIBS)

PKG_CHECK_MODULES([CONFUSE], [libconfuse >= 2.5])
AC_SUBST(CONFUSE_CFLAGS)
AC_SUBST(CONFUSE_LDFLAGS)
AC_SUBST(CONFUSE_LIBS)

AM_PATH_LIBPOPT
AC_SUBST(LIBPOPT_CFLAGS)
AC_SUBST(LIBPOPT_LDFLAGS)
AC_SUBST(LIBPOPT_LIBS)

dnl Mandatory functions and headers
AC_CHECK_HEADERS([stdlib.h unistd.h stdio.h string.h unistd.h])
AC_CHECK_FUNCS([perror strerror readlink dirname])

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

AC_MSG_CHECKING([if compiler supports __attribute__((constructor))])
AC_COMPILE_IFELSE([
	void ctor() __attribute__((constructor));
	void ctor() { };
	], [
		AC_MSG_RESULT([yes])
	], [
		AC_MSG_FAILURE([no, unable to build unieject])
	])

AC_MSG_CHECKING([if compiler supports __attribute__((format(printf, n, n)))])
AC_COMPILE_IFELSE([
	void __attribute__((format(printf, 1, 2))) printflike(const char *fmt, ...) { }
	], [
		AC_MSG_RESULT([yes])
		AC_DEFINE([SUPPORT_ATTRIBUTE_FORMAT], 1, [Define this if the compiler supports the format attribute])
	], [
		AC_MSG_RESULT([no])
	])

AC_MSG_CHECKING([if compiler supports __attribute__((visibility("internal")))])
AC_COMPILE_IFELSE([
	void __attribute__((visibility("internal"))) internal_function() { }
	], [
		AC_MSG_RESULT([yes])
		AC_DEFINE([SUPPORT_ATTRIBUTE_INTERNAL], 1, [Define this if the compiler supports the internal visibility attribute])
	], [
		AC_MSG_RESULT([no])
	])

AC_MSG_CHECKING([if compiler supports __attribute__((nonnull()))])
AC_COMPILE_IFELSE([
	void some_function(void *foo, void *bar) __attribute__((nonnull()));
	void some_function(void *foo, void *bar) { }
	], [
		AC_MSG_RESULT([yes])
		AC_DEFINE([SUPPORT_ATTRIBUTE_NONNULL], 1, [Define this if the compiler supports the nonnull attribute])
	], [
		AC_MSG_RESULT([no])
	])

AC_OUTPUT([po/Makefile.in])
AC_OUTPUT([Makefile
	lib/Makefile
	manpages/Makefile
	lib/libunieject.pc])
