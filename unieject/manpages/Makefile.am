unieject.1: unieject.1.txt2man
	@TXT2MAN@ -s 1 -v "Unieject man pages" -t unieject $< > $(top_builddir)/$(subdir)/$@

unieject.conf.5: unieject.conf.5.txt2man
	@TXT2MAN@ -s 5 -v "Unieject man pages" -t unieject.conf $< > $(top_builddir)/$(subdir)/$@

libunieject_setspeed.3: libunieject_setspeed.3.txt2man
	@TXT2MAN@ -s 3 -v "Unieject man pages" -t libunieject_setspeed $< > $(top_builddir)/$(subdir)/$@

EXTRA_DIST = unieject.1.txt2man unieject.conf.5.txt2man \
	libunieject_setspeed.3.txt2man

if HAVE_TXT2MAN
manpages = unieject.1 \
	unieject.conf.5 \
	libunieject_setspeed.3
endif

man_MANS = $(manpages)

CLEANFILES = $(man_MANS)
