EXTRA_DIST = unieject.conf.5.xml unieject.1.xml \
	$(man_MANS) $(html_DATA)

%: %.xml
	xsltproc http://docbook.sourceforge.net/release/xsl-ns/current/manpages/docbook.xsl $<

%.html: %.xml
	xsltproc http://docbook.sourceforge.net/release/xsl-ns/current/xhtml/docbook.xsl $< > $@

html_DATA = unieject.1.html unieject.conf.5.html
man_MANS = unieject.1 unieject.conf.5

DISTCLEANFILES = $(man_MANS) $(html_DATA)
