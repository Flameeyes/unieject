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

int libunieject_setspeed(struct unieject_opts opts)
{
  if ( ! (unieject_get_misccaps(opts) & CDIO_DRIVE_CAP_MISC_SELECT_SPEED) ) {
    g_critical(_("the selected device doesn't have capability to select speed.\n"));
    return -2;
  }
	
  g_message(_("setting CD-ROM speed to %dX\n"), opts.speed);
  driver_return_code_t sts = cdio_set_speed((CdIo_t*)opts.cdio, opts.speed);
	
  return unieject_status(sts);
}
