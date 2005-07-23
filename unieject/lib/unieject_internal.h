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

#define unieject_error \
	if ( opts.verbose != -1 ) fprintf

#define unieject_verbose \
	if ( opts.verbose == 1 ) fprintf

#define unieject_error_p \
	if ( opts->verbose != -1 ) fprintf

#define unieject_verbose_p \
	if ( opts->verbose == 1 ) fprintf

#include <config.h>
#include <string.h>

#include <unieject.h>

// safe strdup
static char *sstrdup(const char *str)
{
	return str ? strdup(str) : NULL;
}

char *simplifylink(const char *progname, const char *link);
char *checkmount(struct unieject_opts opts, char **device);

#endif
