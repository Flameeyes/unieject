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

#include <cdio/mmc.h>

#ifdef FREEBSD_DRIVER
# include <fcntl.h>
# include <errno.h>
# include <sys/ioctl.h>
# include <sys/cdio.h>
#endif

int libunieject_traytoggle(struct unieject_opts *opts)
{
  mmc_cdb_t cdb = { {0, } };
  uint8_t buffer[8] = { 0, };
	
  CDIO_MMC_SET_COMMAND(cdb.field, CDIO_MMC_GPCMD_GET_EVENT_STATUS);
	
  /* Setup to read header, to get length of data */
  CDIO_MMC_SET_READ_LENGTH16(cdb.field, sizeof(buffer));
	
  cdb.field[1] = 1;		/* Information */
  cdb.field[4] = 1 << 4;	/* Media/tray events */
	
  int status = mmc_run_cmd((CdIo_t*)opts->cdio, mmc_timeout_ms, &cdb,
			   SCSI_MMC_DATA_READ, sizeof(buffer), buffer);
	
  if ( status != 0 ) {
    g_critical(_("unable to get the status of the tray.\n"));
    return -1;
  }
	
  /* The first bit of the fifth byte in the read buffer represents the
   * status of the tray: 1 is open, 0 is closed. */
  if ( buffer[5] & 0x1 ) {
    g_message(_("%s: closing tray.\n"), "traytoggle");
    opts->eject = 0;
  } else {
    g_message(_("%s: ejecting.\n"), "traytoggle");
    opts->eject = 1;
  }
	
  return libunieject_eject(opts);
}

int libunieject_eject(struct unieject_opts *opts)
{
  if ( opts->eject ) {
    if ( ! (unieject_get_misccaps(*opts) & CDIO_DRIVE_CAP_MISC_EJECT) ) {
      g_critical(_("the selected device doesn't have eject capabilities.\n"));
      return -2;
    }
  } else {
    if ( ! (unieject_get_misccaps(*opts) & CDIO_DRIVE_CAP_MISC_CLOSE_TRAY) ) {
      g_critical(_("the selected device doesn't have tray close capabilities.\n"));
      return -2;
    }
  }
	
  if ( UNLIKELY(opts->fake) )
    return 0;

#ifdef FREEBSD_DRIVER
  if ( opts->eject ) {
    int devfd = open(opts->device, O_RDONLY);
    if ( UNLIKELY(devfd == -1) ) {
      g_critical(_("unable to open device descriptor [%s].\n"), strerror(errno));
      return -4;
    }
		
    if ( UNLIKELY(ioctl(devfd, CDIOCALLOW) == -1) ) {
      g_critical(_("error in ioctl [%s].\n"), strerror(errno));
      return -5;
    }
		
    close(devfd);
  }
#endif
	
#ifdef FREEBSD_DRIVER
  driver_return_code_t sts;
  if ( strncmp("/dev/cd", opts->device, 7) != 0 ) {
    if ( opts->eject ) {
      CdIo_t *cdio = opts->cdio;
      sts = cdio_eject_media(&cdio);
    } else {
      cdio_destroy(opts->cdio);
      opts->cdio = NULL;
      sts = cdio_close_tray(opts->device, NULL);
    }
  } else
    sts = mmc_start_stop_media((CdIo_t*)opts->cdio, opts->eject, 0, 0);
#elif defined(__APPLE__)
  driver_return_code_t sts;
  if ( opts->eject )
    sts = cdio_eject_media((CdIo_t**)&opts->cdio);
  else {
    cdio_destroy(opts->cdio);
    opts->cdio = NULL;
    sts = cdio_close_tray(opts->device, NULL);
  }
#else
  driver_return_code_t sts = mmc_start_stop_media((CdIo_t*)opts->cdio, opts->eject, 0, 0);
#endif
	
  return unieject_status(sts);
}
