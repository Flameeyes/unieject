SUBDIRS = po lib manpages

bin_PROGRAMS = unieject

INCLUDES = $(all_includes) -I$(top_srcdir)/lib

unieject_SOURCES = unieject.c lib/utils.c
unieject_CFLAGS = $(AM_CFLAGS) \
	-DSYSCONFDIR=\"$(sysconfdir)\" \
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
	$(top_builddir)/lib/libunieject.la

loadcd: unieject
	rm -f loadcd
	$(LN_S) $< $@

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath mkinstalldirs
EXTRA_CLEAN = loadcd

install-exec-hook: loadcd
	rm -f $(DESTDIR)$(bindir)/loadcd
	(cd $(DESTDIR)$(bindir) && $(LN_S) unieject loadcd)
