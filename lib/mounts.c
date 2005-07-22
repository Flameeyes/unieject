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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_LIBGEN_H
#	include <libgen.h>
#endif

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
	
#if defined(HAVE_READLINK) && defined(HAVE_DIRNAME)
	tmp = (char*)malloc(sizeof(char)*1024);
	int c = readlink(normalized, tmp, 1023);
	if ( c != -1)
	{
		tmp[c] = '\0';
		if ( tmp[0] != '/' ) // relative link
		{
			char *copynorm = sstrdup(normalized);
			char *origdir = sstrdup(dirname(copynorm));
			char *newname = NULL;
			asprintf(&newname, "%s/%s", origdir, tmp);
			free(tmp);
			
			tmp = newname;
			
			free(copynorm);
			free(origdir);
		}
		
		unieject_verbose(stdout, "%s: '%s' is a link to '%s'\n", progname, normalized, tmp);
		free(normalized);
		normalized = sstrdup(tmp);
	} else if ( errno != EINVAL ) {
		// EINVAL is returned when the path is not a link
		perror(progname);
	}
	
	free(tmp);
#endif
	
	// TODO: check for mountpoints, devices
	
	return normalized;
}
