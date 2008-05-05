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

#ifndef __UNIEJECT_H__
#define __UNIEJECT_H__

#include <cdio/cdio.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* When building the library, don't hide functions in this header as
 * the are public.
 *
 * If it's available, prefer protected visibility to default
 * visibility during library build, as that will avoid passing through
 * the PLT for in-module calls.
 *
 * When using the library (rather than building), don't set visibility
 * on symbols, they'll be undefined and thus take default visibility
 * by themselves.
 */
#ifdef BUILD_LIBUNIEJECT
# if SUPPORT_ATTRIBUTE_VISIBILITY_PROTECTED
#  define LIBUNIEJECT_PROTECTED __attribute__( ( visibility("protected") ) )
# else
#  define LIBUNIEJECT_PROTECTED __attribute__( ( visibility("default") ) )
# endif
#else
# define LIBUNIEJECT_PROTECTED
#endif

/**
 * @file
 * @brief Base interface to unieject functions
 */

/**
 * @brief Defines the options used to influence libunieject functions
 */
struct unieject_opts {
	gboolean eject;		/**< Will eject or close the tray? */
	gboolean fake;		/**< Don't eject, just show device found. */
	int verbose;		/**< Enable verbose output. */
	gboolean unmount;	/**< Unmount device if occupied. */
	int speed;		/**< Maximum speed to set the device to. */
	gboolean force;		/**< Force device unmount. */
	gboolean caps;		/**< Follow capabilities reported by device */
	int slot;		/**< Slot to set the changer to. */
	
	char *device;		/**< Path of the device to open. */
	char *umount_wrapper;	/**< Umount wrapper to use (like pumount) */
	char *progname;		/**< Name of the program to use for outputs */
	char *accessmethod;	/**< Access method to load for libcdio */
	void *cdio;		/**< Shaded CdIo_t pointer, don't touch! */
};

/**
 * @brief Gets the default CDIO device name.
 *
 * @note This should be internal only
 */
char *libunieject_defaultdevice() LIBUNIEJECT_PROTECTED;

/**
 * @brief Gets the device name, resolving the argument or finding the default.
 * @param opts Options to apply.
 * @param basename Name of the device to canonicalize or NULL to find the default.
 */
char *libunieject_getdevice(struct unieject_opts opts, const char *basename) LIBUNIEJECT_PROTECTED;

/**
 * @brief Open a given device
 * @param opts Structure to get the options from and set the open pointer to.
 * @retval true Open executed successfully, opts->cdio has the right pointer.
 * @retval false Open has not executed.
 *
 * This is just a wrap-on function for cdio_open, which does a bit of tricks
 * for known-broken operating systems (like FreeBSD).
 */
bool libunieject_open(struct unieject_opts *opts) LIBUNIEJECT_PROTECTED;

/**
 * @brief Eject the media in the passed cdio descriptor.
 * @param opts Pointer to options to apply
 *
 * @retval 0 Eject successful
 * @retval -2 Drive doesn't has the capabilities required
 * @retval -3 Error during ejection
 *
 * @note It's possible that opts->cdio value is set to NULL (and destroyed)
 *       after a call to this method. This because on some OS you must close
 *       previous opened device to do an eject/trayclose.
 */
int libunieject_eject(struct unieject_opts *opts) LIBUNIEJECT_PROTECTED;

/**
 * @brief Toggle the tray between closed and open
 * @param opts Pointer to options to apply
 *
 * @retval 0 Eject successful
 * @retval -2 Drive doesn't has the capabilities required
 * @retval -3 Error during ejection
 *
 * @note This function is simply a wrapper to @ref libunieject_eject() function,
 *       that checks if the tray is open or closed before actually calling the
 *       close or open function. This relies on the hardware being able to
 *       provide those information (MMC-2).
 */
int libunieject_traytoggle(struct unieject_opts *opts) LIBUNIEJECT_PROTECTED;

/**
 * @brief Sets the speed of the given CD-ROM device
 * @param opts Options to apply
 *
 * @retval 0 Speed set successfully.
 * @retval -2 Drive doesn't has the capabilities required
 * @retval -3 Error suring speed setting
 */
int libunieject_setspeed(struct unieject_opts opts) LIBUNIEJECT_PROTECTED;

/**
 * @brief Change the slot of a CD-ROM changer
 * @param opts Options to apply
 *
 * @retval 0 Slot set successfully.
 * @retval -2 Drive doesn't has the capabilities required
 * @retval -3 Error suring slot setting
 */
int libunieject_slotchange(struct unieject_opts opts) LIBUNIEJECT_PROTECTED;

/**
 * @brief Toggle locking of a CD-ROM device
 * @param opts Pointer to options to apply
 * @param lock 1 if you want to lock the drive, 0 if you want to unlock it
 *
 * @retval 0 Locking successful.
 * @retval -2 Drive doesn't have the capabilities required
 * @retval -3 Error during lock toggling
 * @retval -4 Unable to open raw descriptor to the device (some OS only)
 * @retval -5 Unable to execute raw ioctl on the device (some OS only)
 *
 * @note It's possible that opts->cdio value is set to NULL (and destroyed)
 *       after a call to this method. This because on some OS you must close
 *       previous opened device to do an eject/trayclose.
 */
int libunieject_togglelock(struct unieject_opts *opts, int lock) LIBUNIEJECT_PROTECTED;

/**
 * @brief Unmount a device
 * @param opts Options to apply.
 * @param device Normalized name of the device to unmount.
 *
 * @retval true Unmount completed successfully (or opts.fake enabled).
 * @retval false Error unmounting the device.
 *
 * @note While @c device is not modified, passing it as const is not possible
 * as it calls an internal function using non-const char*
 */
bool libunieject_umountdev(struct unieject_opts opts, char *device) LIBUNIEJECT_PROTECTED;

#ifdef __cplusplus
}
#endif

#endif
