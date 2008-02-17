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

#include <config.h>

#include <unieject.h>
#include <unieject_internal.h>

#include <cdio/device.h>

#include <stdio.h>

#ifdef __linux__
#	define DEFAULT_ACCESS "READ_CD"
#else
#	define DEFAULT_ACCESS NULL
#endif

bool libunieject_open(struct unieject_opts *opts)
{

#ifdef FREEBSD_DRIVER
	if ( strncmp("/dev/cd", opts->device, 7) )
		opts->cdio = cdio_open_am(opts->device, DRIVER_FREEBSD, "ioctl");
	else
#endif
	opts->cdio = cdio_open_am(opts->device, cdio_os_driver, opts->accessmethod ? opts->accessmethod : DEFAULT_ACCESS);

	if ( UNLIKELY(!opts->cdio) )
	{
		g_critical(_("cannot find CD-Rom driver.\n"));
		return false;
	}

	return true;
}

