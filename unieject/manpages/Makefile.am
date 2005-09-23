EXTRA_DIST = unieject.mxml unieject.conf.mxml $(man_MANS)

%.1: %.mxml
	rm -f $@
	mxml2man $< $@

%.5: %.mxml
	rm -f $@
	mxml2man $< $@

man_MANS = unieject.1 unieject.conf.5

