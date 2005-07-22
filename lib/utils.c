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

#include <errno.h>

#ifdef HAVE_LIBGEN_H
#	include <libgen.h>
#endif

char *simplifylink(const char *progname, const char *orig)
{
#if defined(HAVE_READLINK) && defined(HAVE_DIRNAME)
	char *tmp = (char*)malloc(sizeof(char)*1024);
	int c = readlink(orig, tmp, 1023);
	if ( c != -1)
	{
		tmp[c] = '\0';
		if ( tmp[0] != '/' ) // relative link
		{
			char *copylink = sstrdup(orig);
			char *origdir = sstrdup(dirname(copylink));
			char *newname = NULL;
			asprintf(&newname, "%s/%s", origdir, tmp);
			free(tmp);
			
			tmp = newname;
			
			free(copylink);
			free(origdir);
		}
		
		return tmp;
	} else if ( errno != EINVAL ) {
		// EINVAL is returned when the path is not a link
		perror(progname);
	}
#endif
	return orig;
}
