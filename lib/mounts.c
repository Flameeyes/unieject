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

gchar *libunieject_defaultdevice()
{
	CdIo_t *cdio = cdio_open(NULL, DRIVER_UNKNOWN);
	char *device_cdio = cdio_get_default_device(cdio);
	/* Need to do this so that g_free() works */
	gchar *device = g_strdup(device_cdio);
	cdio_destroy(cdio);
	free(device_cdio);
	
	return device;
}

char *libunieject_getdevice(const char *basename)
{
	char *normalized = g_strdup(basename);
	char *tmp = NULL; // used to free the right pointers
	
	// compatibility with BSD's eject command
	if ( ! normalized )
	{
		normalized = g_strdup(getenv("EJECT"));
		if ( normalized )
			g_message(_("using value of EJECT variable '%s'\n"), normalized);
	}

#ifdef FREEBSD_DRIVER
	if ( ! normalized )
	{
		normalized = g_strdup("cd0");
		g_message(_("using FreeBSD default: 'cd0'\n"));
	}
#endif

	if ( ! normalized )
	{
		normalized = libunieject_defaultdevice();
		if ( ! normalized ) {
			g_critical(_("no default device identified, exiting.\n"));
			return NULL;
		} else
			g_message(_("using default device '%s'\n"), normalized);
	}

	g_message(_("device name is '%s'\n"), normalized);
	
	if ( normalized[0] != '/' )
	{
		// TODO: this needs to check if there's a node in the relative name, before

		tmp = normalized;
		normalized = g_strdup_printf("/dev/%s", tmp);
		g_free(tmp); tmp = NULL;
	}
	
	g_message(_("expanded name is '%s'\n"), normalized);
	
	tmp = simplifylink(normalized);
	if ( strcmp(tmp, normalized) )
	{
		g_message(_("'%s' is a link to '%s'\n"), normalized, tmp);
		g_free(normalized);
		normalized = tmp;
		tmp = NULL;
	} else
		g_free(tmp);
	
	int len = strlen(normalized);
	if ( normalized[len-1] == '/' )
		normalized[len-1] = '\0';
	
	char *mnt = checkmount(&normalized);
	free(mnt);
	
	/* Now check if the device is a partition or a device itself */
	tmp = rootdevice(normalized);
	if ( tmp ) /* If the device is a partition rather than a root device */
	{
		if ( tmp == (void*)-1 ) return NULL;
	
		g_message(_("'%s' is a partition of device '%s'\n"), normalized, tmp);
		g_free(normalized);
		normalized = tmp;
		tmp = NULL;
	}
	
	g_message(_("device is '%s'\n"), normalized);
	return normalized;
}

bool libunieject_umountdev(struct unieject_opts opts, char *device)
{
	if ( ! opts.unmount ) return true;
	
	if ( opts.umount_wrapper )
	{
		char *cmd = g_strdup_printf("%s %s", opts.umount_wrapper, device);
		int res = 0;
		g_message(_("executing '%s' as umount wrapper.\n"), opts.umount_wrapper);
		res = ( ! opts.fake ) ? system(cmd) : 0;
		
		if ( res != 0 )
			g_message(_("error executing umount wrapper, ignoring.\n"));
		g_free(cmd);
	}
	
	return internal_umountdev(opts, device);
}
