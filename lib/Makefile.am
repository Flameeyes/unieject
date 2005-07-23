libincludedir = $(includedir)/unieject
libinclude_HEADERS = unieject.h

noinst_HEADERS = unieject_internal.h

if LINUX
platformsrcs = linux.c
else
if FREEBSD
platformsrcs = freebsd.c
endif
endif

lib_LTLIBRARIES = libunieject.la
libunieject_la_SOURCES = \
	eject.c \
	mounts.c \
	utils.c \
	speed.c \
	open.c \
	slotchanger.c \
	$(platformsrcs)

libunieject_la_CFLAGS = $(AM_CFLAGS) $(LIBCDIO_CFLAGS)
libunieject_la_LDFLAGS = $(AM_LDFLAGS) $(LIBCDIO_LDFLAGS) --no-undefined
libunieject_la_LIBADD = $(AM_LDADD) $(LIBCDIO_LIBS)

libunieject_la_CURRENT := 2
libunieject_la_REVISION := 0
libunieject_la_AGE := 0
