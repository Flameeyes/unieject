SUBDIRS = po

localedir = $(datadir)/locale

INCLUDES = $(all_includes) -I$(top_srcdir)/lib
DEFS = -DLOCALEDIR=\"$(localedir)\" \
	-DSYSCONFDIR=\"$(sysconfdir)\"

pkginclude_HEADERS = lib/unieject.h

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

libunieject_la_CURRENT := 5
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
unieject_SOURCES = unieject.c
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

EXTRA_DIST = bootstrap.sh \
	m4/attributes.m4 \
	m4/doxygen.m4 \
	m4/pkg.m4 \
	m4/with_pkg.m4
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

EXTRA_DIST += \
	manpages/unieject.conf.5.xml \
	manpages/unieject.1.xml

manpages/%: manpages/%.xml
	xsltproc http://docbook.sourceforge.net/release/xsl-ns/current/manpages/docbook.xsl $<

manpages/%.html: manpages/%.xml
	xsltproc http://docbook.sourceforge.net/release/xsl-ns/current/xhtml/docbook.xsl $< > $@

dist_html_DATA = manpages/unieject.1.html manpages/unieject.conf.5.html
dist_man_MANS = manpages/unieject.1 manpages/unieject.conf.5

MAINTAINERCLEANFILES = $(dist_man_MANS) $(dist_html_DATA)

install-exec-hook: loadcd
	rm -f $(DESTDIR)$(bindir)/loadcd
	(cd $(DESTDIR)$(bindir) && $(LN_S) unieject loadcd)

distcheck-hook: ChangeLog
	( fgrep -q "Unable to regenerate" ChangeLog && exit 1 || exit 0 )
