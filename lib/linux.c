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

char *checkmount(char **device)
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
			g_message(_("'%s' is mounted as '%s'\n"), *device, newmnt);
			ret = newmnt;
			
			free(newdev);
			break;
		}

		if ( strcmp(newmnt, *device) == 0 )
		{
			g_message(_("'%s' is the mount point of '%s'\n"), *device, newdev);
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

static bool internal_umount_partition(struct unieject_opts opts, char *device)
{
	char *mnt = NULL;
	
	if ( opts.fake )
	{
		g_message(_("unmount '%s'\n"), device);
		return true;
	}
	
	while ( ( mnt = checkmount(&device) ) )
	{
#ifdef HAVE_UMOUNT2
		if ( UNLIKELY(umount2(mnt, opts.force ? MNT_FORCE : 0) == -1) )
#else
		if ( UNLIKELY(umount(mnt) == -1) )
#endif
		{
			g_critical("unable to unmount '%s' [%s]\n", mnt, strerror(errno));
			return false;
		}
		
		g_critical("'%s' unmounted from '%s'\n", device, mnt);
	}

	return true;
}

/* Implement this as a bypass, to unmount all the partitions */
bool internal_umountdev(struct unieject_opts opts, char *device)
{
	if ( ! internal_umount_partition(opts, device) ) return false;
	
	gchar *rootdev = rootdevice(device);
	if ( ! rootdev ) rootdev = device;
	
	gchar *glob_target = g_strdup_printf("/sys/block/%s/*", rootdev + (sizeof("/dev/")-1));
	const size_t glob_target_len = strlen(glob_target)-1; /* -1 is for the * character for the glob */
	
	glob_t partitions;
	int r = glob(glob_target, GLOB_NOESCAPE|GLOB_ONLYDIR, NULL, &partitions);
	if ( r == 0)
	{
		g_message(_("unmounting partitions of '%s'.\n"), rootdev);
		for(size_t i = 0; i < partitions.gl_pathc; i++)
		{
			char *directory = partitions.gl_pathv[i] + glob_target_len;
			
			if ( ! strcmp("holders", directory) ) continue;
			if ( ! strcmp("queue", directory) ) continue;
			if ( ! strcmp("slaves", directory) ) continue;
			if ( ! strcmp("device", directory) ) continue;
			if ( ! strcmp("subsystem", directory) ) continue;
			
			gchar *partition = g_strdup_printf("/dev/%s", directory );
			
			if ( ! internal_umount_partition(opts, partition) )
			{
				g_free(partition);
				return false;
			}
			
			g_free(partition);
		}
	}
	g_free(glob_target);
	globfree(&partitions);
	
	if ( rootdev != device ) g_free(rootdev);
	
	return true;
}

gchar *rootdevice(char *device)
{
	struct stat devstat;
	int r = stat(device, &devstat);
	
	if ( r != 0 )
	{
		g_critical(_("unable to stat '%s' [%s]\n"), device, strerror(errno));
		return (void*)-1;
	}
	
	if ( ! S_ISBLK(devstat.st_mode) )
	{
		g_critical(_("'%s' is not a block device.\n"), device);
		return (void*)-1;
	}
	
	dev_t rootminor = -1;
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
		g_critical(_("'%s' is neither an IDE nor a SCSI device or partition, unable to eject.\n"), device);
		return (void*)-1;
	}
	
	if ( minor(devstat.st_rdev) == rootminor )
	{
		g_message(_("'%s' is a proper device (%u,%u).\n"), device, major(devstat.st_rdev), minor(devstat.st_rdev));
		return NULL;
	}
	
	g_critical(_("'%s' is a partition of %u,%lu not a device.\n"), device, major(devstat.st_rdev), rootminor);
	
	/* This code is for Linux 2.6 only... */
	glob_t block_devices;
	r = glob("/sys/block/*", GLOB_NOESCAPE|GLOB_ONLYDIR, NULL, &block_devices);
	if ( r == 0 )
	{
		g_message(_("using sysfs to identify the root device for '%s'.\n"), device);
		
		for(size_t i = 0; i < block_devices.gl_pathc; i++)
		{
			char sysfs_file_dev[ strlen(block_devices.gl_pathv[i]) + 4 + 1 ];
			
			sprintf(sysfs_file_dev, "%s/dev", block_devices.gl_pathv[i]);
			
			FILE *dev_fd = fopen(sysfs_file_dev, "r");
			if ( ! dev_fd ) continue;
			
			unsigned int major, minor;
			fscanf(dev_fd, "%u:%u\n", &major, &minor);
			
			fclose(dev_fd);

			if ( major == major(devstat.st_rdev) && minor == rootminor )
			{
				gchar *rootdev = g_strdup_printf("/dev/%s", block_devices.gl_pathv[i] + (sizeof("/sys/block/") -1));
				globfree(&block_devices);
				
				return rootdev;
			}

		}
	}
	globfree(&block_devices);
	
	if ( isdigit(device[strlen(device)-1]) )
	{
		g_message(_("trying empirical way to find root device for '%s'.\n"), device);
		
		gchar *rootdev = g_strdup(device);
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
