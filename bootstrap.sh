#!/bin/bash

touch ChangeLog README INSTALL NEWS
libtoolize --copy --force
aclocal -I m4 --force
autoconf --force
autoheader --force
automake --add-missing --force-missing  --copy
