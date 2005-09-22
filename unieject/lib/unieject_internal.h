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

#include <config.h>
#include <string.h>

#include <unieject.h>

#ifdef SUPPORT_ATTRIBUTE_FORMAT
#	define PRINTF_LIKE(x, y)	__attribute__( ( format(printf, x, y) ) )
#else
#	define PRINTF_LIKE(x, y)
#endif

#ifdef SUPPORT_ATTRIBUTE_INTERNAL
#	define INTERNAL			__attribute__( ( visibility("internal") ) )
#else
#	define INTERNAL
#endif

#ifdef SUPPORT_ATTRIBUTE_NONNULL
#	define NONNULL(...)		__attribute__( ( nonnull(__VA_ARGS__) ) )
#else
#	define NONNULL(...)
#endif

/* safe strdup */
#define sstrdup(str)			str ? strdup(str) : NULL;

char INTERNAL *simplifylink(const char *link) NONNULL();
char INTERNAL *checkmount(struct unieject_opts opts, char **device) NONNULL();
bool INTERNAL internal_umountdev(struct unieject_opts opts, char *device) NONNULL();

void INTERNAL unieject_error(const struct unieject_opts opts, const char *format, ...) PRINTF_LIKE(2, 3);
void INTERNAL unieject_verbose(const struct unieject_opts opts, const char *format, ...) PRINTF_LIKE(2, 3);
int INTERNAL unieject_status(const struct unieject_opts opts, int sts);

/* Gettext stuff */
#include <gettext.h>
#ifdef ENABLE_NLS
#	define _(x) gettext(x)
#else
#	define _(x) x
#endif

#endif
