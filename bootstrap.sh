#!/bin/bash

libtoolize
touch ChangeLog README INSTALL NEWS

autoreconf -I m4 -i

