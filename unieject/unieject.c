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

#define _GNU_SOURCE

#include <unistd.h>

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cdio/cdio.h>
#include <cdio/mmc.h>
#include <cdio/logging.h>

#include <popt.h>

#include <unieject.h>

static char *progname;

/*
  eject -V                              -- display program version and exit
  eject [-vnrsfq] [<name>]              -- eject device
  eject [-vn] -d                        -- display default device
  eject [-vn] -a on|off|1|0 [<name>]    -- turn auto-eject feature on or off
  eject [-vn] -c <slot> [<name>]        -- switch discs on a CD-ROM changer
  eject [-vn] -x <speed> [<name>]       -- set CD-ROM max speed
Options:
*  -r    -- eject CD-ROM
*  -s    -- eject SCSI device
*  -f    -- eject floppy
*  -q    -- eject tape
  -m    -- do not unmount device even if it is mounted
Long options:
  -h --help   -v --verbose       -d --default
  -a --auto   -c --changerslot  -x --cdspeed
  -r --cdrom  -s --scsi  -f --floppy
  -q --tape   -n --noop  -V --version
  -m --no-unmount
Parameter <name> can be a device file or a mount point.
If omitted, name defaults to `cdrom'.
By default tries -r, -s, -f, and -q in order until success.
*/

struct unieject_opts opts;

enum {
	OP_IGNORE,
	OP_DEFAULT, // --default
};

#define pre_cleanup() \
	free ( progname ); \
	if ( opts.device ) free(opts.device);

/* Parse a all options. */
static bool parse_options (int argc, const char *argv[])
{
	bool retval = false;
	char opt; /* used for argument parsing */
	char *psz_my_source;
	
	opts.eject = 1;
	opts.fake = 0;
	opts.verbose = 0;
	opts.unmount = 1;
	opts.speed = 0;
	opts.device = NULL;
  
	struct poptOption optionsTable[] = {
		{ "trayclose",		't', POPT_ARG_VAL, &opts.eject, 0,
		  "Close CD-Rom tray." },
		{ "noop", 		'n', POPT_ARG_VAL, &opts.fake, 1,
		  "Don't eject, just show device found." },
		{ "default",		'd', POPT_ARG_NONE, NULL, OP_DEFAULT,
		  "Display default device." },
		{ "verbose",		'v', POPT_ARG_VAL, &opts.verbose, 1,
		  "Enable verbose output." },
		{ "no-unmount",		'm', POPT_ARG_VAL, &opts.unmount, 0,
		  "Do not umount device even if it is mounted." },
		{ "quiet",		"Q", POPT_ARG_VAL, &opts.verbose, -1,
		  "Disable output of error messages." },
		
		{ "proc",		'p', POPT_ARG_NONE, NULL, OP_IGNORE,
		  "Ignored (classic eject compatibility)." },
		POPT_AUTOHELP {NULL, 0, 0, NULL, 0}
	};

	poptContext optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
	
	progname = strrchr(argv[0],'/');
	progname = progname ? strdup(progname+1) : strdup(argv[0]);
	
	while ((opt = poptGetNextOpt (optCon)) >= 0)
	{
		switch(opt)
		{
		case OP_DEFAULT: {
				CdIo_t *p_cdio = cdio_open (NULL, DRIVER_UNKNOWN);

				char *default_device = cdio_get_default_device(p_cdio);
				printf("%s: default device: `%s'\n", progname, default_device);
				retval = true;
				
				free(default_device);
				cdio_destroy(p_cdio);
			}
		}
	}
	
	if (opt < -1)
	{
		/* an error occurred during option processing */
		fprintf(stderr, "%s: %s\n",
			poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
			poptStrerror(opt));
		pre_cleanup();
		exit (EXIT_FAILURE);
	}
	
	const char *arg_device = poptGetArg(optCon);
	if ( arg_device )
	{
		if ( strncmp(arg_device, "/dev/", 5) != 0 )
		{
			if ( arg_device[0] != '/' ) // consider it a devicename without /dev/ prefix
				asprintf(&opts.device, "/dev/%s", arg_device);
		} else {
			opts.device = strdup(arg_device);
		}
	}
	
	if ( opts.verbose && poptGetArg(optCon) )
		printf("%s: further non-option arguments ignored.\n", progname);
	
	return retval;
}

#define cleanup() \
	if ( p_cdio ) cdio_destroy(p_cdio); \
	pre_cleanup()

int do_eject();

int main(int argc, const char *argv[])
{
#ifndef NDEBUG
	cdio_loglevel_default = 0;
#endif

	// if options completes the close, exit
	if ( parse_options(argc, argv) )
	{
		free(progname);
		return 0;
	}
	
	CdIo_t *p_cdio = cdio_open (opts.device, DRIVER_UNKNOWN);

	if (p_cdio == NULL)
	{
		fprintf(stderr, "%s: cannot find CD-Rom driver.\n", progname);
		cleanup();
		return -1;
	}
	
	if ( opts.fake || opts.verbose )
	{
		char *default_device = cdio_get_default_device(p_cdio);
		printf("%s: device is: `%s'\n", progname, default_device);
		free(default_device);
	}
	
	int ret;
	if ( opts.speed == 0 )
		ret = libunieject_eject(progname, opts, &p_cdio);
	else
		ret = set_speed(&p_cdio);
	
	cleanup();

	return ret;
}

int set_speed(CdIo_t *p_cdio)
{
}
