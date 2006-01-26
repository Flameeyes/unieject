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
	$(CONFUSE_CFLAGS) \
	$(C99FLAGS)

unieject_LDFLAGS = $(AM_LDFLAGS) \
	$(LIBCDIO_LDFLAGS) \
	$(LIBPOPT_LDFLAGS) \
	$(CONFUSE_LDFLAGS) \
	$(EXP_LDFLAGS)

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
AUTOMAKE_OPTIONS = foreign no-dist-gzip dist-bzip2

EXTRA_DIST = config.rpath mkinstalldirs ChangeLog NEWS COPYING INSTALL \
	README configure AUTHORS ABOUT-NLS bootstrap.sh unieject.conf.sample \
	svn2cl.xsl
EXTRA_CLEAN = loadcd

DISTCLEANFILES = ChangeLog

cltimestamp:
	touch cltimestamp

ChangeLog: cltimestamp
	@if test -d $(top_srcdir)/.svn; then \
		svn up $(top_srcdir); \
		svn log $(top_srcdir) --xml --verbose | \
			xsltproc $(top_srcdir)/svn2cl.xsl - | \
		$(GSED) -f $(top_srcdir)/svn2authors.sed \
		> ChangeLog; \
	else \
		touch ChangeLog; \
	fi
	rm cltimestamp

install-exec-hook: loadcd
	rm -f $(DESTDIR)$(bindir)/loadcd
	(cd $(DESTDIR)$(bindir) && $(LN_S) unieject loadcd)
