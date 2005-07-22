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

#include <unieject.h>
#include <unieject_internal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *libunieject_defaultdevice(const char *progname, struct unieject_opts opts)
{
	CdIo_t *cdio = cdio_open(NULL, DRIVER_UNKNOWN);
	char *device = cdio_get_default_device(cdio);
	cdio_destroy(cdio);
	
	return device;
}

char *libunieject_getdevice(const char *progname, struct unieject_opts opts, const char *basename)
{
	char *normalized = basename ? strdup(basename) : NULL;
	char *tmp = NULL; // used to free the right pointers
	
#ifdef HAVE_GETENV
	// compatibility with BSD's eject command
	if ( ! normalized )
	{
		normalized = sstrdup(getenv("EJECT"));
		if ( normalized )
			unieject_verbose(stdout, "%s: using value of EJECT variable '%s'\n", progname, normalized);
	}
#endif

	if ( ! normalized )
	{
		normalized = libunieject_defaultdevice(progname, opts);
		unieject_verbose(stdout, "%s: using default device '%s'\n", progname, normalized);
	}

	unieject_verbose(stdout, "%s: device name is '%s'\n", progname, normalized);
	
	if ( normalized[0] != '/' )
	{
		// TODO: this needs to check if there's a node in the relative name, before

		tmp = normalized;
		asprintf(&normalized, "/dev/%s", tmp);
		free(tmp); tmp = NULL;
	}
	
	unieject_verbose(stdout, "%s: expanded name is '%s'\n", progname, normalized);
	
	// TODO: check for links, mountpoints, devices
	
	return normalized;
}
