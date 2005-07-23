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

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <cdio/cdio.h>
#include <cdio/mmc.h>
#include <cdio/logging.h>

#include <popt.h>

static char *progname;

/*
  eject -V                              -- display program version and exit
  eject [-vn] -a on|off|1|0 [<name>]    -- turn auto-eject feature on or off
  eject [-vn] -c <slot> [<name>]        -- switch discs on a CD-ROM changer
Options:
*  -r    -- eject CD-ROM
*  -s    -- eject SCSI device
*  -f    -- eject floppy
*  -q    -- eject tape
Long options:
  -a --auto   -c --changerslot
  -r --cdrom  -s --scsi  -f --floppy
  -q --tape   -V --version

By default tries -r, -s, -f, and -q in order until success.
*/

struct unieject_opts opts;

enum {
	OP_IGNORE,
	OP_DEFAULT, // --default
	OP_SPEED,
	OP_CHANGER,
	OP_ERROR
};

#define pre_cleanup() \
	free ( progname ); \
	if ( opts.device ) free(opts.device);

/* Parse a all options. */
static int parse_options (int argc, const char *argv[])
{
	char tmpopt;
	char opt = OP_IGNORE; /* used for argument parsing */
	char *psz_my_source;
	
	opts.eject = 1;
	opts.fake = 0;
	opts.verbose = 0;
	opts.unmount = 1;
	opts.speed = 0;
	opts.force = 0;
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
		{ "quiet",		'Q', POPT_ARG_VAL, &opts.verbose, -1,
		  "Disable output of error messages." },
		{ "force",		'f', POPT_ARG_VAL, &opts.force, 1,
		  "Force unmount of device." },
		{ "speed",		'x', POPT_ARG_INT, &opts.speed, OP_SPEED,
		  "Set CD-Rom max speed." },
		{ "changerslot",	'c', POPT_ARG_NONE, NULL, OP_CHANGER,
		  "Switch discs on a CD-ROM changer." },
		
		{ "proc",		'p', POPT_ARG_NONE, NULL, OP_IGNORE,
		  "Ignored (classic eject compatibility)." },
		POPT_AUTOHELP {NULL, 0, 0, NULL, 0}
	};

	poptContext optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
	
	progname = strrchr(argv[0],'/');
	progname = progname ? strdup(progname+1) : strdup(argv[0]);
	
	while ((tmpopt = poptGetNextOpt (optCon)) >= 0)
	{
		if ( tmpopt == OP_IGNORE ) continue;
		
		if ( opt != OP_IGNORE )
		{
			unieject_error(stderr, "%s: you can use just one of -x, -c and -d options\n", progname);
			return OP_ERROR;
		} else
			opt = tmpopt;
	}
	
	if (opt < -1)
	{
		/* an error occurred during option processing */
		fprintf(stderr, "%s: %s\n",
			poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
			poptStrerror(opt));
		return OP_ERROR;
	}
	
	const char *arg_device = poptGetArg(optCon);
	
	if ( poptGetArg(optCon) )
		unieject_verbose(stdout, "%s: further non-option arguments ignored.\n", progname);
	
	opts.device = libunieject_getdevice(progname, opts, arg_device);
	
	return opt;
}

#define cleanup() \
	if ( cdio ) cdio_destroy(cdio); \
	pre_cleanup()

int do_eject();

int main(int argc, const char *argv[])
{
#ifndef NDEBUG
	cdio_loglevel_default = 0;
#endif

	int what = parse_options(argc, argv);
	
	// First switch, non-device options
	switch(what)
	{
	case OP_ERROR:
		return -1;
	case OP_DEFAULT: {
			char *default_device = libunieject_defaultdevice(progname, opts);
			printf("%s: default device: `%s'\n", progname, default_device);
			
			free(default_device);
			return 0;
		}
	}
	
	CdIo_t *cdio = libunieject_opendevice(progname, opts);
	if ( ! cdio )
	{
		cleanup();
		return -1;
	}

	int retval;
	switch(what)
	{
		case OP_SPEED:
			retval = libunieject_setspeed(progname, opts, cdio);
			break;
		default:
			if ( ! libunieject_umountdev(progname, opts, opts.device) )
			{
				unieject_error(stderr, "%s: unable to unmount device '%s'.\n", progname, opts.device);
				retval = -4;
			} else
				retval = libunieject_eject(progname, opts, cdio);
	}
	
	cleanup();
	return retval;
}
