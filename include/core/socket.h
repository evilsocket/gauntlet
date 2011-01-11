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
#ifndef __SOCKET_H
#	define __SOCKET_H

#include "network.h"
#include "log.h"

class Socket {
	protected :
		int			   m_sd;
		fd_set		   m_fdset;
		unsigned long  m_polltime;
		unsigned long  m_timeout;
		unsigned long  m_address;
		unsigned short m_port;
		char		   m_hostname[0xFF];
		char		   m_ipaddress[0xFF];
		
	public :
	
		Socket();
		Socket( int sd );
		Socket( char *hostname, unsigned short port );
		
		~Socket();
		
		int   sd();
		
		void  setAddress( unsigned long address );
		char *getAddress();
		
		net_retcode_t setNonBlocking();
		
		net_retcode_t connect();
		net_retcode_t connect( char *hostname, unsigned short port );
		
		net_retcode_t read  ( unsigned char *buffer, unsigned int size, int *pread );
		net_retcode_t write ( unsigned char *buffer, unsigned int size, int *pwritten );
};

#endif

