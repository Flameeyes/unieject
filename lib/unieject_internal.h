/* unieject - Universal eject command
   Copyright (C) 2005, Diego Pettenò

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   */

#ifndef __UNIEJECT_INTERNAL_H__
#define __UNIEJECT_INTERNAL_H__

// We need this for asprintf
#define _GNU_SOURCE

#include <config.h>
#include <string.h>

#include <unieject.h>

#ifdef __GNUC__
#	define PRINTF_LIKE(x, y)	__attribute__( ( format(printf, x, y) ) )
#	define INTERNAL			__attribute__( ( visibility("internal") ) )
#else
#	define INTERNAL
#	define PRINTF_LIKE(x, y)
#endif

// safe strdup
static char *sstrdup(const char *str)
{
	return str ? strdup(str) : NULL;
}

char INTERNAL *simplifylink(const char *progname, const char *link);
char INTERNAL *checkmount(struct unieject_opts opts, char **device);
bool INTERNAL internal_umountdev(struct unieject_opts opts, const char *device);

void INTERNAL unieject_error(const struct unieject_opts opts, const char *format, ...) PRINTF_LIKE(2, 3);
void INTERNAL unieject_verbose(const struct unieject_opts opts, const char *format, ...) PRINTF_LIKE(2, 3);

// Gettext stuff
#include <gettext.h>
#ifdef ENABLE_NLS
#	define _(x) gettext(x)
#else
#	define _(x) x
#endif

#endif
