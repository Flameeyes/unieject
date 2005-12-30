EXTRA_DIST = unieject.conf.5.xml unieject.1.xml $(man_MANS)

%: %.xml
	xmlto man $<

man_MANS = unieject.1 unieject.conf.5

DISTCLEANFILES = $(man_MANS)
