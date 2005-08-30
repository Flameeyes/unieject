SUBDIRS = po lib manpages m4

localedir = $(datadir)/locale

bin_PROGRAMS = unieject

INCLUDES = $(all_includes) -I$(top_srcdir)/lib
DEFS = -DLOCALEDIR=\"$(localedir)\" \
	-DSYSCONFDIR=\"$(sysconfdir)\"

unieject_SOURCES = unieject.c lib/utils.c
unieject_CFLAGS = $(AM_CFLAGS) \
	$(LIBCDIO_CFLAGS) \
	$(LIBPOPT_CFLAGS) \
	$(CONFUSE_CFLAGS)

unieject_LDFLAGS = $(AM_LDFLAGS) \
	$(LIBCDIO_LDFLAGS) \
	$(LIBPOPT_LDFLAGS) \
	$(CONFUSE_LDFLAGS)

unieject_LDADD = $(AM_LDADD) \
	$(LIBCDIO_LIBS) \
	$(LIBPOPT_LIBS) \
	$(CONFUSE_LIBS) \
	@LIBINTL@ \
	$(top_builddir)/lib/libunieject.la

loadcd: unieject
	rm -f loadcd
	$(LN_S) $< $@

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign

EXTRA_DIST = config.rpath mkinstalldirs ChangeLog NEWS COPYING INSTALL \
	README configure AUTHORS ABOUT-NLS bootstrap.sh unieject.conf.sample
EXTRA_CLEAN = loadcd

install-exec-hook: loadcd
	rm -f $(DESTDIR)$(bindir)/loadcd
	(cd $(DESTDIR)$(bindir) && $(LN_S) unieject loadcd)
