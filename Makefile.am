bin_PROGRAMS = unieject

unieject_SOURCES = unieject.c
unieject_CPPFLAGS = -I$(top_srcdir)/lib
unieject_CFLAGS = $(AM_CFLAGS) $(LIBCDIO_CFLAGS) $(LIBPOPT_CFLAGS)
unieject_LDFLAGS = $(AM_LDFLAGS) $(LIBCDIO_LDFLAGS) $(LIBPOPT_LDFLAGS)
unieject_LDADD = $(AM_LDADD) $(LIBCDIO_LIBS) $(LIBPOPT_LIBS)
