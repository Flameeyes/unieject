SUBDIRS = po lib manpages

bin_PROGRAMS = unieject

INCLUDES = $(all_includes) -I$(top_srcdir)/lib

unieject_SOURCES = unieject.c lib/utils.c
unieject_CFLAGS = $(AM_CFLAGS) $(LIBCDIO_CFLAGS) $(LIBPOPT_CFLAGS)
unieject_LDFLAGS = $(AM_LDFLAGS) $(LIBCDIO_LDFLAGS) $(LIBPOPT_LDFLAGS)
unieject_LDADD = $(AM_LDADD) $(LIBCDIO_LIBS) $(LIBPOPT_LIBS) $(top_builddir)/lib/libunieject.la

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath mkinstalldirs
