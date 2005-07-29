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
	OP_VERSION,
	OP_ERROR
};

#define pre_cleanup() \
	free ( opts.progname ); \
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
	opts.accessmethod = NULL;
  
	struct poptOption optionsTable[] = {
		{ "trayclose",		't', POPT_ARG_VAL, &opts.eject, 0,
		  gettext_noop("Close CD-Rom tray.") },
		{ "noop", 		'n', POPT_ARG_VAL, &opts.fake, 1,
		  gettext_noop("Don't eject, just show device found.") },
		{ "default",		'd', POPT_ARG_NONE, NULL, OP_DEFAULT,
		  gettext_noop("Display default device.") },
		{ "verbose",		'v', POPT_ARG_VAL, &opts.verbose, 1,
		  gettext_noop("Enable verbose output.") },
		{ "no-unmount",		'm', POPT_ARG_VAL, &opts.unmount, 0,
		  gettext_noop("Do not umount device even if it is mounted.") },
		{ "quiet",		'Q', POPT_ARG_VAL, &opts.verbose, -1,
		  gettext_noop("Disable output of error messages.") },
		{ "force",		'f', POPT_ARG_VAL, &opts.force, 1,
		  gettext_noop("Force unmount of device.") },
		{ "speed",		'x', POPT_ARG_INT, &opts.speed, OP_SPEED,
		  gettext_noop("Set CD-Rom max speed.") },
		{ "changerslot",	'c', POPT_ARG_NONE, NULL, OP_CHANGER,
		  gettext_noop("Switch discs on a CD-ROM changer.") },
		{ "accessmethod", 	'A', POPT_ARG_STRING, &opts.accessmethod, OP_IGNORE,
		  gettext_noop("Select the access method for libcdio.") },
		{ "debugcdio",		'D', POPT_ARG_INT, &cdio_loglevel_default, OP_IGNORE,
		  gettext_noop("Set debugging level for libcdio.") },
		{ "version",		'V', POPT_ARG_NONE, NULL, OP_VERSION,
		  gettext_noop("Display version and copyright information and exit.") },
		
		{ "proc",		'p', POPT_ARG_NONE, NULL, OP_IGNORE,
		  gettext_noop("Ignored (classic eject compatibility).") },
		POPT_AUTOHELP {NULL, 0, 0, NULL, 0}
	};

	poptContext optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
	
	opts.progname = strrchr(argv[0],'/');
	opts.progname = opts.progname ? strdup(opts.progname+1) : strdup(argv[0]);
	
	while ((tmpopt = poptGetNextOpt (optCon)) >= 0)
	{
		if ( tmpopt == OP_IGNORE ) continue;
		
		if ( opt != OP_IGNORE )
		{
			unieject_error(opts, _("you can use just one of -x, -c and -d options\n"));
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
		unieject_verbose(opts, _("further non-option arguments ignored.\n"));
	
	opts.device = libunieject_getdevice(opts, arg_device);
	
	return opt;
}

#define cleanup() \
	if ( opts.cdio ) cdio_destroy((CdIo_t*)opts.cdio); \
	pre_cleanup()

int main(int argc, const char *argv[])
{
	int what = parse_options(argc, argv);
	
	// First switch, non-device options
	switch(what)
	{
	case OP_ERROR:
		return -1;
	case OP_DEFAULT: {
			char *default_device = libunieject_defaultdevice(opts);
			printf(_("%s: default device: `%s'\n"), opts.progname, default_device);
			
			free(default_device);
			free(opts.progname);
			return 0;
		}
	case OP_VERSION: {
			printf(_("unieject version %s\n"), PACKAGE_VERSION);
			printf(_("Copyright (c) 2005 Diego Pettenò\n"));
			
			free(opts.progname);
			return 0;
		}
	case OP_CHANGER:
	case OP_IGNORE:
		if ( ! libunieject_umountdev(opts, opts.device) )
		{
			unieject_error(opts, _("unable to unmount device '%s'.\n"), opts.device);
			free(opts.progname);
			return -4;
		}
	}
	
	if ( ! libunieject_open(&opts) )
	{
		cleanup();
		return -1;
	}

	int retval;
	switch(what)
	{
		case OP_SPEED:
			retval = libunieject_setspeed(opts);
			break;
		case OP_CHANGER:
			retval = libunieject_slotchange(opts);
			break;
		default:
			retval = libunieject_eject(&opts);
	}
	
	cleanup();
	return retval;
}
