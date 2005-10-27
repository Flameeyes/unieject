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

#include <unieject_internal.h>
#include <unieject.h>

#include <cdio/mmc.h>

#include <stdio.h>

#if defined(__linux__) && defined(USE_LOCK_WORKAROUND)
# include <fcntl.h>
# include <errno.h>
# include <sys/ioctl.h>
# include <linux/cdrom.h>

# ifndef CDROM_LOCKDOOR
#  error "Unable to find CDROM_LOCKDOOR definition, and lock workaround requested"
# endif
#endif

/* Set how long to wait for MMC commands to complete */
#define DEFAULT_TIMEOUT_MS 100000

int libunieject_togglelock(struct unieject_opts *opts, int lock)
{
	if ( opts->caps )
	{
		// TODO: tell libcdio author about this
		cdio_drive_misc_cap_t unused, misc_cap;
		cdio_get_drive_cap((CdIo_t*)opts->cdio, &unused, &unused, &misc_cap);

#ifdef __FreeBSD__
		if ( strncmp("/dev/cd", opts->device, 7) != 0 )
			misc_cap = 0xFFFFFFFF;
#endif

		if ( ! (misc_cap & CDIO_DRIVE_CAP_MISC_LOCK) )
		{
			unieject_error(*opts, _("the selected device doesn't have locking capabilities.\n"));
			return -2;
		}
	}
	
	if ( opts->fake )
		return 0;

/*
 * On Linux the allow medium removal command is a priviledged command that
 * cannot be instanced by userspace.
 * Until this changes, use a workaround with syscalls instead.
 */
#if defined(__linux__) && defined(USE_LOCK_WORKAROUND)
	cdio_destroy(opts->cdio);
	opts->cdio = NULL;
	
	int devfd = open(opts->device, O_RDONLY|O_NONBLOCK);
	if ( UNLIKELY(devfd == -1) )
	{
		unieject_error(*opts, _("unable to open device descriptor [%s].\n"), strerror(errno));
		return -4;
	}
	
	if ( UNLIKELY(ioctl(devfd, CDROM_LOCKDOOR, lock ? 1 : 0) == -1) )
	{
		unieject_error(*opts, _("error in ioctl [%s].\n"), strerror(errno));
		return -5;
	}
	
	close(devfd);
	
	return 0;
#else
	mmc_cdb_t lockcmd = { {0, } }; /* Command description buffer */
	uint8_t buf[1]; /* Fake buffer */
	
	CDIO_MMC_SET_COMMAND(lockcmd.field, CDIO_MMC_GPCMD_ALLOW_MEDIUM_REMOVAL);
	
	lockcmd.field[4] = lock ? 1 : 0;
	
	driver_return_code_t sts = mmc_run_cmd(
		(CdIo_t*)opts->cdio,
		DEFAULT_TIMEOUT_MS, /* Should this be configurable? */
		&lockcmd,
		SCSI_MMC_DATA_READ,
		0,
		buf
		);
	
	return unieject_status(*opts, sts);
#endif
}
