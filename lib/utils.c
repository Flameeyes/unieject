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

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>

#include <glib.h>

#include <cdio/device.h>


char *simplifylink(const char *orig)
{
	char tmp[1024];
	
	int c = readlink(orig, tmp, 1023);
	if ( c != -1)
	{
		tmp[c] = '\0';
		if ( tmp[0] != '/' ) // relative link
		{
			char *copylink = g_strdup(orig);
			char *origdir = g_strdup(dirname(copylink));
			g_free(copylink);
			
			char *newname = g_strdup_printf("%s/%s", origdir, tmp);
			g_free(origdir);

			return newname;
		}
		
		return g_strdup(tmp);
	}
	
 	return g_strdup(orig);
}

/**
 * @brief Outputs a message about the given status return code
 * @param sts Status return code from libcdio library
 */
int unieject_status(int sts)
{
	switch(sts)
	{
	case DRIVER_OP_SUCCESS:
		return 0;
	default:
		g_critical(_("unable to execute command (unknown CDIO status: %d)\n"), sts);
	}
	
	return -3;
}

/**
 * @brief Returns the (workaround'ed) capabilities of a drive
 *
 * This function is a wrapper around cdio_get_drive_cap() function from libcdio
 * that tries to work around situations like FreeBSD's broken capabilities or
 * USB flash drives passed directly to libcdio.
 */
cdio_drive_misc_cap_t unieject_get_misccaps(const struct unieject_opts opts)
{
	if ( opts.caps )
	{
		cdio_drive_misc_cap_t read_cap, write_cap, misc_cap;
		cdio_get_drive_cap((CdIo_t*)opts.cdio, &read_cap, &write_cap, &misc_cap);
		
		/* In case there's an error reading capabilities or they are
		   not loaded, then return the full caps (act as we were ignoring
		   capabilities.
		   
		   Also return full capabilities if it can't read CD-R, as we
		   assume that every CD-Rom drive reads them, and if it doesn't
		   report it, it might be an USB/SCSI device. */
		if ( misc_cap & (CDIO_DRIVE_CAP_UNKNOWN|CDIO_DRIVE_CAP_ERROR) ||
		     ! (read_cap & CDIO_DRIVE_CAP_READ_CD_R) )
			return 0xFFFFFFFF;
		
#ifdef FREEBSD_DRIVER
		if ( strncmp("/dev/cd", opts.device, 7) != 0 )
			return 0xFFFFFFFF;
#endif
		return misc_cap;
	}
	
	return 0xFFFFFFFF;
}
