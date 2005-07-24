#!/bin/bash

export WANT_AUTOCONF_2_5="yes"
export WANT_AUTOMAKE_1_9="yes"
export WANT_AUTOCONF="2.5"
export WANT_AUTOMAKE="1.9"

touch ChangeLog
libtoolize --copy --force
gettextize --no-changelog --copy --force

aclocal -I m4
autoconf --force
autoheader --force
automake --add-missing --force-missing  --copy
