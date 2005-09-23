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

#include <config.h>

#include <unieject_internal.h>
#include <unieject.h>

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <cdio/cdio.h>
#include <cdio/mmc.h>
#include <cdio/logging.h>

#include <popt.h>

#ifdef HAVE_LIBCONFUSE
#include <confuse.h>
#endif

#include <gettext.h>
#ifdef ENABLE_NLS
#include <locale.h>
#endif

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
	OP_LOCK,
	OP_UNLOCK,
	OP_ERROR
};

void init_opts() __attribute__((constructor));
void cleanup() __attribute__((destructor));

/* Initialize the default options (set as constructor) */
void init_opts()
{
	opts.eject = 1;
	opts.fake = 0;
	opts.verbose = 0;
	opts.unmount = 1;
	opts.speed = 0;
	opts.force = 0;
	opts.caps = 1;
	opts.device = NULL;
	opts.umount_wrapper = NULL;
	opts.accessmethod = NULL;
}

void cleanup()
{
	if ( opts.progname ) free(opts.progname);
	if ( opts.device ) free(opts.device);
	if ( opts.cdio ) cdio_destroy((CdIo_t*)opts.cdio);
}

#ifdef HAVE_LIBCONFUSE
static void parse_configuration()
{
	cfg_opt_t cfgopts[] =
	{
		CFG_SIMPLE_STR("device", &opts.device),
		CFG_SIMPLE_INT("verbosity", &opts.verbose),
		CFG_SIMPLE_BOOL("unmount", &opts.unmount),
		CFG_SIMPLE_BOOL("force", &opts.force),
		CFG_SIMPLE_STR("accessmethod", &opts.accessmethod),
		CFG_SIMPLE_INT("debugcdio", &cdio_loglevel_default),
		CFG_SIMPLE_BOOL("respect-capabilities", &opts.caps),
		CFG_SIMPLE_BOOL("unmount-wrapper", &opts.umount_wrapper),
		CFG_END()
	};
	
	cfg_t *cfg = cfg_init(cfgopts, CFGF_NONE);
	if ( cfg_parse(cfg, SYSCONFDIR "/unieject.conf") == CFG_PARSE_ERROR )
	{
		unieject_error(opts, _("Error parsing configuration file %s\n"), SYSCONFDIR "/unieject.conf");
		cfg_free(cfg);
		exit(-5);
	}
	
	char *userconf;
	asprintf(&userconf, "%s/.unieject", getenv("HOME"));
	if ( cfg_parse(cfg, userconf) == CFG_PARSE_ERROR )
	{
		unieject_error(opts, _("Error parsing configuration file %s\n"), userconf);
		free(userconf);
		cfg_free(cfg);
		exit(-5);
	}
	
	free(userconf);
	cfg_free(cfg);
}
#endif

/* Parse a all options. */
static int parse_options (int argc, const char *argv[])
{
	char tmpopt;
	char opt = OP_IGNORE; /* used for argument parsing */
	
	struct poptOption optionsTable[] = {
		{ "trayclose",		't', POPT_ARG_VAL, &opts.eject, 0,
		  gettext_noop("Close CD-Rom tray."), NULL },
		{ "speed",		'x', POPT_ARG_INT, &opts.speed, OP_SPEED,
		  gettext_noop("Set CD-Rom max speed."),
		  "max_speed" },
		{ "changerslot",	'c', POPT_ARG_NONE, NULL, OP_CHANGER,
		  gettext_noop("Switch discs on a CD-ROM changer."),
		  "changer" },
		{ "lock",		'l', POPT_ARG_NONE, NULL, OP_LOCK,
		  gettext_noop("Lock the CD-Rom drive."), NULL },
		{ "unlock",		'L', POPT_ARG_NONE, NULL, OP_UNLOCK,
		  gettext_noop("Unlock the CD-Rom drive."), NULL },
		
		{ "noop", 		'n', POPT_ARG_VAL, &opts.fake, 1,
		  gettext_noop("Don't eject, just show device found."), NULL },
		{ "default",		'd', POPT_ARG_NONE, NULL, OP_DEFAULT,
		  gettext_noop("Display default device."), NULL },
		
		{ "verbose",		'v', POPT_ARG_VAL, &opts.verbose, 1,
		  gettext_noop("Enable verbose output."), NULL },
		{ "quiet",		'Q', POPT_ARG_VAL, &opts.verbose, -1,
		  gettext_noop("Disable output of error messages."), NULL },
		
		{ "no-unmount",		'm', POPT_ARG_VAL, &opts.unmount, 0,
		  gettext_noop("Do not umount device even if it is mounted."), NULL },
		{ "unmount",		'u', POPT_ARG_VAL, &opts.unmount, 1,
		  gettext_noop("Unmount device if mounted (default behavior)."), NULL },
		
		{ "force",		'f', POPT_ARG_VAL, &opts.force, 1,
		  gettext_noop("Force unmount of device."), NULL },
		{ "no-force",		0, POPT_ARG_VAL, &opts.force, 0,
		  gettext_noop("Don't force unmount of device (default behavior)."), NULL },
		
		{ "ignore-caps",	0, POPT_ARG_VAL, &opts.caps, 0,
		  gettext_noop("Ignore capabilities as reported by device."), NULL },
		{ "no-ignore-caps",	0, POPT_ARG_VAL, &opts.caps, 1,
		  gettext_noop("Don't ignore capabilities reported by device (default behavior)."), NULL },
		
		{ "umount-wrapper",	'W', POPT_ARG_STRING, &opts.umount_wrapper, OP_IGNORE,
		  gettext_noop("Use an umount wrapper to unmount."),
		  "wrapper" },
		
		{ "accessmethod", 	'A', POPT_ARG_STRING, &opts.accessmethod, OP_IGNORE,
		  gettext_noop("Select the access method for libcdio."),
		  "method" },
		{ "debugcdio",		'D', POPT_ARG_INT, &cdio_loglevel_default, OP_IGNORE,
		  gettext_noop("Set debugging level for libcdio."),
		  "level" },
		
		{ "version",		'V', POPT_ARG_NONE, NULL, OP_VERSION,
		  gettext_noop("Display version and copyright information and exit."), NULL },
		
		{ "proc",		'p', POPT_ARG_NONE, NULL, OP_IGNORE,
		  gettext_noop("Ignored (classic eject compatibility)."), NULL },
		POPT_AUTOHELP {NULL, 0, 0, NULL, 0, NULL, NULL}
	};

	poptContext optCon = poptGetContext (NULL, argc, argv, optionsTable, 0);
	
	opts.progname = strrchr(argv[0],'/');
	opts.progname = opts.progname ? strdup(opts.progname+1) : strdup(argv[0]);
	
	while ((tmpopt = poptGetNextOpt (optCon)) >= 0)
	{
		if ( tmpopt == OP_IGNORE ) continue;
		
		if ( opt != OP_IGNORE )
		{
			unieject_error(opts, _("you can use just one of -x, -c, -l, -L and -d options\n"));
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
		
		exit(-1);
	}
	
	const char *arg_device = poptGetArg(optCon);
	if ( ! arg_device )
		arg_device = opts.device;
	
	if ( poptGetArg(optCon) )
		unieject_verbose(opts, _("further non-option arguments ignored.\n"));
	
	opts.device = libunieject_getdevice(opts, arg_device);
	
	return opt;
}

int main(int argc, const char *argv[])
{
#ifdef ENABLE_NLS
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
#endif

#ifdef HAVE_LIBCONFUSE
	parse_configuration();
#endif
	int what = parse_options(argc, argv);
	
	/*
	 * To make simpler the user experience with unieject, it's possible to
	 * provide a "loadcd" symlink or alias that defaults to trayclose
	 * instead of eject.
	 */
	if ( strcmp("loadcd", opts.progname) == 0 )
	{
		unieject_verbose(opts, _("default to closing tray instead of eject.\n"));
		opts.eject = 0;
	}
	
	// First switch, non-device options
	switch(what)
	{
	case OP_DEFAULT: {
			char *default_device = libunieject_defaultdevice();
			printf(_("%s: default device: `%s'\n"), opts.progname, default_device);
			
			return 0;
		}
	case OP_VERSION: {
			printf(_("unieject version %s\n"), PACKAGE_VERSION);
			printf(_("Copyright (c) 2005 Diego Pettenò\n"));
			
			return 0;
		}
	case OP_CHANGER:
	case OP_IGNORE:
		if ( ! libunieject_umountdev(opts, opts.device) )
		{
			unieject_error(opts, _("unable to unmount device '%s'.\n"), opts.device);
			return -4;
		}
	case OP_ERROR:
		return -1;
	}
	
	if ( ! libunieject_open(&opts) )
		return -1;

	int retval;
	switch(what)
	{
		case OP_SPEED:
			retval = libunieject_setspeed(opts);
			break;
		case OP_CHANGER:
			retval = libunieject_slotchange(opts);
			break;
		case OP_LOCK:
			retval = libunieject_togglelock(&opts, 1);
			break;
		case OP_UNLOCK:
			retval = libunieject_togglelock(&opts, 0);
			break;
		default:
			retval = libunieject_eject(&opts);
	}
	
	return retval;
}
