/* posix-util.c - Utility functions for Posix
 *	Copyright (C) 2001 Werner Koch (dd9jn)
 *      Copyright (C) 2001 g10 Code GmbH
 *
 * This file is part of GPGME.
 *
 * GPGME is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GPGME is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#include <config.h>
#ifndef HAVE_DOSISH_SYSTEM

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util.h"


const char *
_gpgme_get_gpg_path (void)
{
    /* #warning Forced to take GPG development version
     *  return "/home/wk/work/gnupg-stable/g10/gpg";
     */

    return GPG_PATH;
}


#endif /*!HAVE_DOSISH_SYSTEM*/





