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

#include <cdio/cdio.h>
#include <cdio/mmc.h>

#include <stdio.h>

int libunieject_eject(char *progname, struct unieject_opts opts, CdIo_t *p_cdio)
{
	// TODO: tell libcdio author about this
	cdio_drive_misc_cap_t unused, i_misc_cap;
	cdio_get_drive_cap(p_cdio, &unused, &unused, &i_misc_cap);
	
	if ( opts.eject )
	{
		if ( ! (i_misc_cap & CDIO_DRIVE_CAP_MISC_EJECT) )
		{
			if ( opts.verbose != -1 )
				fprintf(stderr, "%s: the selected device doesn't have eject capabilities.\n", progname);
			return -2;
		}
	} else {
		if ( ! (i_misc_cap & CDIO_DRIVE_CAP_MISC_CLOSE_TRAY) )
		{
			if ( opts.verbose != -1 )
				fprintf(stderr, "%s: the selected device doesn't have tray close capabilities.\n", progname);
			return -2;
		}
	}
	
	if ( ! opts.fake )
	{
		driver_return_code_t r_action = mmc_start_stop_media(p_cdio, opts.eject, 0, 0);
		if ( r_action != DRIVER_OP_SUCCESS )
		{
			if ( opts.verbose != -1 )
				fprintf(stderr, "%s: unable to execute command (CDIO returned: %d)\n", progname, r_action);
			return -3;
		}
	}
}
