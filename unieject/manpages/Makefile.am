unieject.1: unieject.1.txt2man
	txt2man -s 1 -v "Unieject man pages" -t unieject $< > $(top_builddir)/$(subdir)/$@

unieject.conf.5: unieject.conf.5.txt2man
	txt2man -s 5 -v "Unieject man pages" -t unieject.conf $< > $(top_builddir)/$(subdir)/$@

libunieject_setspeed.3: libunieject_setspeed.3.txt2man
	txt2man -s 3 -v "Unieject man pages" -t libunieject_setspeed $< > $(top_builddir)/$(subdir)/$@

man_MANS = unieject.1 \
	unieject.conf.5 \
	libunieject_setspeed.3

CLEANFILES = $(man_MANS)
