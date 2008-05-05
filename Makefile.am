SUBDIRS = po manpages m4

localedir = $(datadir)/locale

INCLUDES = $(all_includes) -I$(top_srcdir)/lib
DEFS = -DLOCALEDIR=\"$(localedir)\" \
	-DSYSCONFDIR=\"$(sysconfdir)\"

libincludedir = $(includedir)/unieject
libinclude_HEADERS = lib/unieject.h

noinst_HEADERS = lib/unieject_internal.h lib/gettext.h

if LINUX
platformsrcs = lib/linux.c
endif
if FREEBSD
platformsrcs = lib/freebsd.c
endif

lib_LTLIBRARIES = libunieject.la
libunieject_la_SOURCES = \
	lib/eject.c \
	lib/mounts.c \
	lib/utils.c \
	lib/speed.c \
	lib/open.c \
	lib/slotchanger.c \
	lib/lock.c \
	$(platformsrcs)

libunieject_la_CURRENT := 4
libunieject_la_REVISION := 0
libunieject_la_AGE := 0

libunieject_la_CFLAGS = $(AM_CFLAGS) $(LIBCDIO_CFLAGS) $(GLIB_CFLAGS)	\
	-DG_LOG_DOMAIN=\"libunieject\" -DBUILD_LIBUNIEJECT
libunieject_la_LDFLAGS = $(AM_LDFLAGS) $(EXP_LDFLAGS) $(LIBCDIO_LDFLAGS) \
	-version-info $(libunieject_la_CURRENT):$(libunieject_la_REVISION):$(libunieject_la_AGE) \
libunieject_la_LIBADD = $(AM_LDADD) $(LIBCDIO_LIBS) $(GLIB_LIBS) $(LTLIBINTL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = lib/libunieject.pc

bin_PROGRAMS = unieject
unieject_SOURCES = unieject.c lib/utils.c
unieject_CFLAGS = $(AM_CFLAGS) \
	$(LIBCDIO_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(CONFUSE_CFLAGS)

unieject_LDFLAGS = $(AM_LDFLAGS) \
	$(LIBCDIO_LDFLAGS) \
	$(CONFUSE_LDFLAGS) \
	$(EXP_LDFLAGS)

unieject_LDADD = $(AM_LDADD) \
	$(LIBCDIO_LIBS) \
	$(GLIB_LIBS) \
	$(CONFUSE_LIBS) \
	$(LTLIBINTL) \
	libunieject.la

loadcd: unieject
	rm -f loadcd
	$(LN_S) $< $@

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign no-dist-gzip dist-bzip2

EXTRA_DIST = config.rpath mkinstalldirs configure bootstrap.sh
EXTRA_CLEAN = loadcd

dist_doc_DATA = ChangeLog NEWS COPYING INSTALL README AUTHORS	\
	ABOUT-NLS unieject.conf.sample DONATING

DISTCLEANFILES = ChangeLog

cltimestamp:
	touch cltimestamp

if REGEN_CHANGELOG
ChangeLog: cltimestamp
	@if test -d $(top_srcdir)/.git; then \
		GIT_DIR="$(top_srcdir)/.git" git log --summary \
		> ChangeLog; \
	else \
		touch ChangeLog; \
	fi
	rm cltimestamp
else
ChangeLog:
	echo "Unable to regenerate" > ChangeLog
endif

install-exec-hook: loadcd
	rm -f $(DESTDIR)$(bindir)/loadcd
	(cd $(DESTDIR)$(bindir) && $(LN_S) unieject loadcd)

distcheck-hook: ChangeLog
	( fgrep -q "Unable to regenerate" ChangeLog && exit 1 || exit 0 )
