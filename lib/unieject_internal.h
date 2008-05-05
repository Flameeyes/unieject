/* unieject - Universal eject command
   Copyright (C) 2005-2006, Diego Pettenò

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with unieject; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef __UNIEJECT_INTERNAL_H__
#define __UNIEJECT_INTERNAL_H__

#include <config.h>
#include <string.h>

#include <unieject.h>

#if SUPPORT_ATTRIBUTE_FORMAT
# define PRINTF_LIKE(x, y)	__attribute__( ( format(printf, x, y) ) )
#else
# define PRINTF_LIKE(x, y)
#endif

#if SUPPORT_ATTRIBUTE_VISIBILITY_INTERNAL
# define INTERNAL		__attribute__( ( visibility("internal") ) )
#elif SUPPORT_ATTRIBUTE_VISIBILITY_HIDDEN
# define HIDDEN			__attribute__( ( visibility("hidden") ) )
#else
# define INTERNAL
#endif

#if SUPPORT_ATTRIBUTE_NONNULL
# define NONNULL(...)		__attribute__( ( nonnull(__VA_ARGS__) ) )
#else
# define NONNULL(...)
#endif

#if SUPPORT__BUILTIN_EXPECT
# define LIKELY(x)		__builtin_expect(!!(x), 1)
# define UNLIKELY(x)		__builtin_expect(!!(x), 0)
#else
# define LIKELY(x)		x
# define UNLIKELY(x)		x
#endif

/* safe strdup */
#define sstrdup(str)			str ? strdup(str) : NULL;

char INTERNAL *simplifylink(const char *link) NONNULL();
char INTERNAL *checkmount(struct unieject_opts opts, char **device) NONNULL();
bool INTERNAL internal_umountdev(struct unieject_opts opts, char *device) NONNULL();

/**
 * @brief Returns the root device, if the given device is actually a partition
 * @param opts Options to apply.
 * @param device Device to check
 */
char INTERNAL *rootdevice(struct unieject_opts opts, char *device) NONNULL();

int INTERNAL unieject_status(const struct unieject_opts opts, int sts);
cdio_drive_misc_cap_t INTERNAL unieject_get_misccaps(const struct unieject_opts opts);

/* Gettext stuff */
#include <gettext.h>
#ifdef ENABLE_NLS
#	define _(x) dgettext("unieject", x)
#else
#	define _(x) x
#endif

#endif
