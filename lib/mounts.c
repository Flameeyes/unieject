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

#include <unieject_internal.h>
#include <unieject.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *libunieject_defaultdevice()
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
			unieject_verbose(opts, _("using value of EJECT variable '%s'\n"), normalized);
	}
#endif

#ifdef FREEBSD_DRIVER
	if ( ! normalized )
	{
		normalized = strdup("cd0");
		unieject_verbose(opts, _("using FreeBSD default: 'cd0'\n"));
	}
#endif

	if ( ! normalized )
	{
		normalized = libunieject_defaultdevice();
		if ( ! normalized ) {
			unieject_error(opts, _("no default device identified, exiting.\n"));
			return NULL;
		} else
			unieject_verbose(opts, _("using default device '%s'\n"), normalized);
	}

	unieject_verbose(opts, _("device name is '%s'\n"), normalized);
	
	if ( normalized[0] != '/' )
	{
		// TODO: this needs to check if there's a node in the relative name, before

		tmp = normalized;
		asprintf(&normalized, "/dev/%s", tmp);
		free(tmp); tmp = NULL;
	}
	
	unieject_verbose(opts, _("expanded name is '%s'\n"), normalized);
	
	tmp = simplifylink(normalized);
	if ( strcmp(tmp, normalized) )
	{
		unieject_verbose(opts, _("'%s' is a link to '%s'\n"), normalized, tmp);
		free(normalized);
		normalized = tmp;
		tmp = NULL;
	}
	
	int len = strlen(normalized);
	if ( normalized[len-1] == '/' )
		normalized[len-1] = '\0';
	
	char *mnt = checkmount(opts, &normalized);
	free(mnt);
	
	/* Now check if the device is a partition or a device itself */
	tmp = rootdevice(opts, normalized);
	if ( tmp ) /* If the device is a partition rather than a root device */
	{
		if ( tmp == (void*)-1 ) return NULL;
	
		unieject_verbose(opts, _("'%s' is a partition of device '%s'\n"), normalized, tmp);
		free(normalized);
		normalized = tmp;
		tmp = NULL;
	}
	
	unieject_verbose(opts, _("device is '%s'\n"), normalized);
	return normalized;
}

bool libunieject_umountdev(struct unieject_opts opts, char *device)
{
	if ( opts.fake || ! opts.unmount ) return true;
	
	if ( opts.umount_wrapper )
	{
		char *cmd = NULL;
		int res = 0;
		unieject_verbose(opts, _("executing '%s' as umount wrapper.\n"), opts.umount_wrapper);
		asprintf(&cmd, "%s %s", opts.umount_wrapper, device);
		
		res = system(cmd);
		
		if ( res != 0 )
			unieject_verbose(opts, _("error executing umount wrapper, ignoring.\n"));
		free(cmd);
	}
	
	return internal_umountdev(opts, device);
}
