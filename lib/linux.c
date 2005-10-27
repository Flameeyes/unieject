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

#include <stdlib.h>
#include <stdio.h>
#include <sys/mount.h>
#include <errno.h>

char *checkmount(struct unieject_opts opts, char **device)
{
	char *ret = NULL;
	FILE *fp = fopen("/proc/mounts", "r");
	
	char *mnt = (char*)malloc(1024);
	char *dev = (char*)malloc(1024);
	
	while ( fscanf(fp, "%s %s %*s %*s %*d %*d\n", dev, mnt) != EOF )
	{
		if ( dev[0] != '/' ) continue;
		
		char *newdev = simplifylink(dev);
		char *newmnt = simplifylink(mnt);
		
		// symlinks?
		if ( strcmp(newdev, *device) == 0 )
		{
			unieject_verbose(opts, _("'%s' is mounted as '%s'\n"), *device, newmnt);
			ret = newmnt;
			
			free(newdev);
			break;
		}

		if ( strcmp(newmnt, *device) == 0 )
		{
			unieject_verbose(opts, _("'%s' is the mount point of '%s'\n"), *device, newdev);
			ret = *device;
			*device = newdev;
			
			free(newmnt);
			break;
		}
		
		free(newdev);
		free(newmnt);
	}
	free(mnt);
	free(dev);
	
	fclose(fp);
	return ret;
}

bool internal_umountdev(struct unieject_opts opts, char *device)
{
	struct unieject_opts nonverbose_opts = opts;
	nonverbose_opts.verbose = 0;
	
	char *mnt = NULL;
	
	while ( ( mnt = checkmount(opts, &device) ) )
	{
#ifdef HAVE_UMOUNT2
		if ( UNLIKELY(umount2(mnt, opts.force ? MNT_FORCE : 0) == -1) )
#else
		if ( UNLIKELY(umount(mnt) == -1) )
#endif
		{
			unieject_error(opts, "unable to unmount '%s' [%s]\n", mnt, strerror(errno));
			return false;
		}
		
		unieject_verbose(opts, "'%s' unmounted from '%s'\n", device, mnt);
	}

	return true;
}
