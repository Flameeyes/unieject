/* unieject - Universal eject command
   Copyright (C) 2005-2008, Diego Pettenò

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

#include <unieject_internal.h>
#include <unieject.h>

#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>

#include <cdio/cdio.h>
#include <cdio/mmc.h>
#include <cdio/logging.h>

#include <glib.h>

#ifdef HAVE_LIBCONFUSE
#include <confuse.h>
#endif

#include <gettext.h>
#ifdef HAVE_SETLOCALE
#include <locale.h>
#endif

static struct unieject_opts opts = {
	.eject = 1,
	.fake = 0,
	.verbose = 0,
	.unmount = 1,
	.speed = 0,
	.force = 0,
	.caps = 1,
	.device = NULL,
	.umount_wrapper = NULL,
	.accessmethod = NULL
};

enum {
	OP_IGNORE,
	OP_DEFAULT, // --default
	OP_SPEED,
	OP_CHANGER,
	OP_VERSION,
	OP_LOCK,
	OP_UNLOCK,
	OP_TOGGLE,
	OP_ERROR
};

void cleanup() __attribute__((destructor));

void cleanup()
{
	free(opts.progname);
	free(opts.device);
	if ( opts.cdio ) cdio_destroy((CdIo_t*)opts.cdio);
}

#ifdef HAVE_LIBCONFUSE
static void parse_configuration()
{
	static cfg_opt_t cfgopts[] =
	{
		CFG_SIMPLE_STR("device", &opts.device),
		CFG_SIMPLE_INT("verbosity", &opts.verbose),
		CFG_SIMPLE_BOOL("unmount", &opts.unmount),
		CFG_SIMPLE_BOOL("force", &opts.force),
		CFG_SIMPLE_STR("accessmethod", &opts.accessmethod),
		CFG_SIMPLE_INT("debugcdio", &cdio_loglevel_default),
		CFG_SIMPLE_BOOL("respect-capabilities", &opts.caps),
		CFG_SIMPLE_STR("unmount-wrapper", &opts.umount_wrapper),
		CFG_END()
	};
	
	cfg_t *cfg = cfg_init(cfgopts, CFGF_NONE);
	if ( cfg_parse(cfg, SYSCONFDIR "/unieject.conf") == CFG_PARSE_ERROR )
	{
		g_critical(_("Error parsing configuration file %s\n"), SYSCONFDIR "/unieject.conf");
		cfg_free(cfg);
		exit(-5);
	}
	
	char *userconf;
	asprintf(&userconf, "%s/.unieject", getenv("HOME"));
	if ( cfg_parse(cfg, userconf) == CFG_PARSE_ERROR )
	{
		g_critical(_("Error parsing configuration file %s\n"), userconf);
		free(userconf);
		cfg_free(cfg);
		exit(-5);
	}
	
	free(userconf);
	cfg_free(cfg);
}
#endif

/* Parse a all options. */
static int parse_options (int argc, char *argv[])
{
	gboolean toggle = 0, changer = 0, lock = 0, unlock = 0,
	  display_device = 0, verbose = 0, quiet = 0, version = 0;
	gchar **remaining_options = NULL;
	
        GOptionEntry optionsTable[] = {
	  { "trayclose",		't', G_OPTION_FLAG_REVERSE, G_OPTION_ARG_NONE, &opts.eject,
	    gettext_noop("Close CD-Rom tray."), NULL },
	  { "traytoggle",		'T', 0, G_OPTION_ARG_NONE, &toggle,
	    gettext_noop("Toggle tray open/close."), NULL },
	  { "speed",		'x', 0, G_OPTION_ARG_INT, &opts.speed,
	    gettext_noop("Set CD-Rom max speed."), "max_speed" },
	  { "changerslot",	'c', 0, G_OPTION_ARG_NONE, &changer,
	    gettext_noop("Switch discs on a CD-ROM changer."), NULL },
	  { "lock",		'l', 0, G_OPTION_ARG_NONE, &lock,
	    gettext_noop("Lock the CD-Rom drive."), NULL },
	  { "unlock",		'L', 0, G_OPTION_ARG_NONE, &unlock,
	    gettext_noop("Unlock the CD-Rom drive."), NULL },
		
	  { "noop", 		'n', 0, G_OPTION_ARG_NONE, &opts.fake,
	    gettext_noop("Don't eject, just show device found."), NULL },
	  { "default",		'd', 0, G_OPTION_ARG_NONE, &display_device,
	    gettext_noop("Display default device."), NULL },
		
	  { "verbose",		'v', 0, G_OPTION_ARG_NONE, &verbose,
	    gettext_noop("Enable verbose output."), NULL },
	  { "quiet",		'Q', 0, G_OPTION_ARG_NONE, &quiet,
	    gettext_noop("Disable output of error messages."), NULL },
		
	  { "no-unmount",	'm', G_OPTION_FLAG_REVERSE, G_OPTION_ARG_NONE, &opts.unmount,
	    gettext_noop("Do not umount device even if it is mounted."), NULL },
	  { "unmount",		'u', 0, G_OPTION_ARG_NONE, &opts.unmount,
	    gettext_noop("Unmount device if mounted (default behavior)."), NULL },
		
	  { "force",		'f', 0, G_OPTION_ARG_NONE, &opts.force,
	    gettext_noop("Force unmount of device."), NULL },
	  { "no-force",		0, G_OPTION_FLAG_REVERSE, G_OPTION_ARG_NONE, &opts.force,
	    gettext_noop("Don't force unmount of device (default behavior)."), NULL },
		
	  { "ignore-caps",	0, G_OPTION_FLAG_REVERSE, G_OPTION_ARG_NONE, &opts.caps,
	    gettext_noop("Ignore capabilities as reported by device."), NULL },
	  { "no-ignore-caps",	0, 0, G_OPTION_ARG_NONE, &opts.caps,
	    gettext_noop("Don't ignore capabilities reported by device (default behavior)."), NULL },
		
	  { "umount-wrapper",	'W', 0, G_OPTION_ARG_STRING, &opts.umount_wrapper,
	    gettext_noop("Use an umount wrapper to unmount."),
	    "wrapper" },
		
	  { "accessmethod", 	'A', 0, G_OPTION_ARG_STRING, &opts.accessmethod,
	    gettext_noop("Select the access method for libcdio."),
	    "method" },
	  { "debugcdio",	 'D', 0, G_OPTION_ARG_INT, &cdio_loglevel_default,
	    gettext_noop("Set debugging level for libcdio."),
	    "level" },
		
	  { "version",		'V', 0, G_OPTION_ARG_NONE, &version,
	    gettext_noop("Display version and copyright information and exit."), NULL },
		
	  { "proc",		'p', 0, G_OPTION_ARG_NONE, NULL,
	    gettext_noop("Ignored (classic eject compatibility)."), NULL },
	  { "tape",		'q', 0, G_OPTION_ARG_NONE, NULL,
	    gettext_noop("Ignored"), NULL },
	  { "floppy",		'f', 0, G_OPTION_ARG_NONE, NULL,
	    gettext_noop("Ignored"), NULL },
	  { "cdrom",		'r', 0, G_OPTION_ARG_NONE, NULL,
	    gettext_noop("Ignored"), NULL },
	  { "scsi",		's', 0, G_OPTION_ARG_NONE, NULL,
	    gettext_noop("Ignored"), NULL },
	  { "auto",		'a', 0, G_OPTION_ARG_NONE, NULL,
	    gettext_noop("Ignored"), NULL },
	  { G_OPTION_REMAINING, 0, 0, G_OPTION_ARG_STRING_ARRAY, &remaining_options,
	    NULL, NULL },
	  { NULL }
	};

	GError *error = NULL;
	GOptionContext *context = g_option_context_new("");
	g_option_context_add_main_entries(context, optionsTable, PACKAGE_TARNAME);

	g_option_context_parse(context, &argc, &argv, &error);

	if ( error != NULL ) {
	  g_critical("%s\n", error->message);
	  fprintf(stderr, _("Run '%s --help' to see a full list of available command line options.\n"), argv[0]);
	  exit(-1);
	}

	int opt = OP_IGNORE;

#define UNIEJECT_CHECK_SINGLE_OPT(testval, optval)		\
	if ( testval ) opt = (opt == OP_IGNORE) ? optval : OP_ERROR;

	UNIEJECT_CHECK_SINGLE_OPT(opts.speed, OP_SPEED);
	UNIEJECT_CHECK_SINGLE_OPT(changer, OP_CHANGER);
	UNIEJECT_CHECK_SINGLE_OPT(version, OP_VERSION);
	UNIEJECT_CHECK_SINGLE_OPT(lock, OP_LOCK);
	UNIEJECT_CHECK_SINGLE_OPT(unlock, OP_UNLOCK);
	UNIEJECT_CHECK_SINGLE_OPT(toggle, OP_TOGGLE);

	if ( opt == OP_ERROR ) {
	  g_critical(_("you can use just one of -x, -c, -l, -L and -d options\n"));
	  return opt;
	}

	if ( quiet && verbose ) {
	  opt = OP_ERROR;

	  g_critical(_("you can use just one of -v and -q options\n"));
	  return opt;
	}

	if ( quiet )
	  opts.verbose = -1;
	else if ( verbose )
	  opts.verbose = 1;
	else
	  opts.verbose = 0;
	
	const char *arg_device = opts.device;
	if ( remaining_options && *remaining_options ) {
	  opts.device = *remaining_options;
	  if ( *(remaining_options+1) )
		g_warning(_("further non-option arguments ignored.\n"));
	}

	opts.device = libunieject_getdevice(opts, arg_device);
	if ( ! opts.device ) return OP_ERROR;
	
	return opt;
}

/**
 * @brief Simple function to ignore log messages from glib
 *
 * This function is used to implement verbose level when using glib
 * message logging.
 */
void unieject_g_log_noop() { }

int main(int argc, char *argv[])
{
#ifdef HAVE_SETLOCALE
	setlocale (LC_ALL, "");
#endif

#ifdef ENABLE_NLS
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
#endif

#ifdef HAVE_LIBCONFUSE
	parse_configuration();
#endif
	int what = parse_options(argc, argv);
	
	switch(opts.verbose) {
	case -1:
	  g_log_set_handler("", G_LOG_LEVEL_CRITICAL, unieject_g_log_noop, NULL);
	case 0:
	  g_log_set_handler("", G_LOG_LEVEL_WARNING, unieject_g_log_noop, NULL);
	}
	
	/*
	 * To make simpler the user experience with unieject, it's possible to
	 * provide a "loadcd" symlink or alias that defaults to trayclose
	 * instead of eject.
	 */
	if ( strcmp("loadcd", argv[0]) == 0 )
	{
		g_message(_("default to closing tray instead of eject.\n"));
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
			printf("Copyright (C) 2005-2008 Diego Pettenò\n");
			printf("This is free software.  You may redistribute copies of it under the terms of\n"
				"the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.\n"
				"There is NO WARRANTY, to the extent permitted by law.\n");

			return 0;
		}
	case OP_CHANGER:
	case OP_TOGGLE:
	case OP_IGNORE:
		if ( ! libunieject_umountdev(opts, opts.device) )
		{
			g_critical(_("unable to unmount device '%s'.\n"), opts.device);
			return -4;
		}
		break;
	case OP_ERROR:
		return -1;
	}
	
	if ( UNLIKELY(! libunieject_open(&opts)) )
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
		case OP_TOGGLE:
			retval = libunieject_traytoggle(&opts);
		default:
			retval = libunieject_eject(&opts);
	}
	
	return retval;
}
