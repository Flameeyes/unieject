libincludedir = $(includedir)/unieject
libinclude_HEADERS = unieject.h

noinst_HEADERS = unieject_internal.h

lib_LTLIBRARIES = libunieject.la
libunieject_la_SOURCES = \
	eject.c \
	mounts.c

libunieject_la_CFLAGS = $(AM_CFLAGS) $(LIBCDIO_CFLAGS)
libunieject_la_LDFLAGS = $(AM_LDFLAGS) $(LIBCDIO_LDFLAGS) --no-undefined
libunieject_la_LIBADD = $(AM_LDADD) $(LIBCDIO_LIBS)
