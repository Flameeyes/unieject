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

char *libunieject_defaultdevice(struct unieject_opts opts)
{
	CdIo_t *cdio = cdio_open(NULL, DRIVER_UNKNOWN);
	char *device = cdio_get_default_device(cdio);
	cdio_destroy(cdio);
	
	return device;
}

char *libunieject_getdevice(struct unieject_opts opts, const char *basename)
{
	char *normalized = sstrdup(basename);
	char *tmp = NULL; // used to free the right pointers
	
#ifdef HAVE_GETENV
	// compatibility with BSD's eject command
	if ( ! normalized )
	{
		normalized = sstrdup(getenv("EJECT"));
		if ( normalized )
			unieject_verbose(stdout, _("%s: using value of EJECT variable '%s'\n"), opts.progname, normalized);
	}
#endif

#ifdef __FreeBSD__
	if ( ! normalized )
	{
		normalized = strdup("cd0");
		unieject_verbose(stdout, _("%s: using FreeBSD default: 'cd0'\n"), opts.progname);
	}
#endif

	if ( ! normalized )
	{
		normalized = libunieject_defaultdevice(opts);
		unieject_verbose(stdout, _("%s: using default device '%s'\n"), opts.progname, normalized);
	}

	unieject_verbose(stdout, _("%s: device name is '%s'\n"), opts.progname, normalized);
	
	if ( normalized[0] != '/' )
	{
		// TODO: this needs to check if there's a node in the relative name, before

		tmp = normalized;
		asprintf(&normalized, "/dev/%s", tmp);
		free(tmp); tmp = NULL;
	}
	
	unieject_verbose(stdout, _("%s: expanded name is '%s'\n"), opts.progname, normalized);
	
	tmp = simplifylink(opts.progname, normalized);
	if ( tmp != normalized )
	{
		unieject_verbose(stdout, _("%s: '%s' is a link to '%s'\n"), opts.progname, normalized, tmp);
		free(normalized);
		normalized = tmp;
		tmp = NULL;
	}
	
	int len = strlen(normalized);
	if ( normalized[len-1] == '/' )
		normalized[len-1] = '\0';
	
	char *mnt = checkmount(opts, &normalized);
	
	// TODO: check for mountpoints, devices
	
	unieject_verbose(stdout, _("%s: device is '%s'\n"), opts.progname, normalized);
	return normalized;
}

bool libunieject_umountdev(struct unieject_opts opts, const char *device)
{
	if ( opts.fake || ! opts.unmount ) return true;
	
	return internal_umountdev(opts, device);
}
