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

#include <cdio/device.h>

#include <stdio.h>

bool libunieject_open(struct unieject_opts *opts)
{
#ifdef __FreeBSD__
	if ( strcmp("/dev/cd", opts->device) == 0 )
		opts->cdio = cdio_open(opts->device, DRIVER_FREEBSD);
	else
		opts->cdio = cdio_open_am(opts->device, DRIVER_FREEBSD, "ioctl");
#else
	opts->cdio = cdio_open(opts->device, cdio_os_driver);
#endif

	if ( ! opts->cdio )
	{
		unieject_error_p(stderr, "%s: cannot find CD-Rom driver.\n", opts->progname);
		return false;
	}

	return true;
}

