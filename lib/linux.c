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

#include <unieject_internal.h>
#include <unieject.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <glob.h>

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

char *rootdevice(struct unieject_opts opts, char *device)
{
	struct stat devstat;
	int r = stat(device, &devstat);
	
	if ( r != 0 )
	{
		unieject_error(opts, _("unable to stat '%s' [%s]\n"), device, strerror(errno));
		return (void*)-1;
	}
	
	if ( ! S_ISBLK(devstat.st_mode) )
	{
		unieject_error(opts, _("'%s' is not a block device.\n"), device);
		return (void*)-1;
	}
	
	int rootminor = -1;
	switch( major(devstat.st_rdev) )
	{
	case 3:  /* IDE Devices: every 64 minors there's a new device */
		rootminor = minor(devstat.st_rdev) & ~63;
		break;
	case 8:  /* SCSI Disk devices: every 16 minors there's a new device */
		rootminor = minor(devstat.st_rdev) & ~15;
		break;
	case 9:  /* SCSI Tape devices */
	case 11: /* SCSI CD-ROM devices */
		return NULL;
	default:
		unieject_error(opts, _("'%s' is neither an IDE nor a SCSI device or partition, unable to eject.\n"), device);
		return (void*)-1;
	}
	
	if ( minor(devstat.st_rdev) == rootminor )
	{
		unieject_verbose(opts, _("'%s' is a proper device (%u,%u).\n"), device, major(devstat.st_rdev), minor(devstat.st_rdev));
		return NULL;
	}
	
	unieject_error(opts, _("'%s' is a partition of %u,%u not a device.\n"), device, major(devstat.st_rdev), rootminor);
	
	/* This code is for Linux 2.6 only... */
	glob_t block_devices;
	r = glob("/sys/block/*", GLOB_NOESCAPE|GLOB_ONLYDIR, NULL, &block_devices);
	if ( r == 0 ) {
		unieject_verbose(opts, _("using sysfs to identify the root device for '%s'.\n"), device);
		
		for(int i = 0; i < block_devices.gl_pathc; i++) {
			char sysfs_file_dev[ strlen(block_devices.gl_pathv[i]) + 4 + 1 ];
			
			sprintf(sysfs_file_dev, "%s/dev", block_devices.gl_pathv[i]);
			
			FILE *dev_fd = fopen(sysfs_file_dev, "r");
			if ( ! dev_fd ) continue;
			
			unsigned int major, minor;
			fscanf(dev_fd, "%u:%u\n", &major, &minor);
			
			fclose(dev_fd);

			if ( major == major(devstat.st_rdev) && minor == rootminor ) {
				char *rootdev = malloc( strlen(block_devices.gl_pathv[i]) - (sizeof("/sys/block/")-1) + sizeof("/dev/") );
				sprintf(rootdev, "/dev/%s", block_devices.gl_pathv[i] + (sizeof("/sys/block/") -1));
				globfree(&block_devices);
				
				return rootdev;
			}

		}
	}
	globfree(&block_devices);
	
	if ( isdigit(device[strlen(device)-1]) )
	{
		unieject_verbose(opts, _("trying empirical way to find root device for '%s'.\n"), device);
		
		char *rootdev = sstrdup(device);
		while ( isdigit(rootdev[strlen(rootdev)-1]) )
			rootdev[strlen(rootdev)-1] = '\0';
	
		struct stat rootstat;
		r = stat(rootdev, &rootstat);
		
		/* Don't consider all the possible ways this might fail, just
		   check if it worked or not. */
		if ( r == 0 &&
		     S_ISBLK(rootstat.st_mode) &&
		     major(rootstat.st_rdev) == major(devstat.st_rdev) &&
		     minor(rootstat.st_rdev) == rootminor
		   )
			return rootdev;
	}
	
	return (void*)-1;
}
