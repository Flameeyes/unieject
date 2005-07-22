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

#ifndef __UNIEJECT_H__
#define __UNIEJECT_H__

/**
 * @file
 * @brief Base interface to unieject functions
 */

/**
 * @brief Defines the options to 
 */
struct opt_s {
	int eject;		// Will eject or close the tray?
	int fake;		// Don't eject, just show device found.
	int verbose;		// Enable verbose output.
	int unmount;		// Unmount device if occupied
	int speed;		// Maximum speed to set the device to
	
	char *device;		// Device to open
};

#endif