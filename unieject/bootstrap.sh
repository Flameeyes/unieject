#!/bin/bash

libtoolize
touch ChangeLog README INSTALL NEWS

ACLOCAL="aclocal -I m4" \
autoreconf -i

