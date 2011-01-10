/*
 * This file is part of the Gauntlet security system.
 *
 * Copyleft of Simone Margaritelli aka evilsocket <evilsocket@gmail.com>
 *
 * Hybris is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Hybris is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hybris.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __NETWORK_H
#	define __NETWORK_H

#include <netdb.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>

typedef enum
{
   NET_FAILED = -1,
   NET_OK,
   NET_NOADDR,
   NET_TIMEDOUT,
   NET_BADARGS,
   NET_MOREDATA,
   NET_CLIENTDISCONNECT,
   NET_NOTSUPPORTED
}
net_retcode_t;

#endif
