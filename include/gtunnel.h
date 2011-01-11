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
#ifndef __GTUNNEL_H
#	define __GTUNNEL_H

#include "listener.h"
#include "config.h"
#include <dlfcn.h>

#define G_MODULE_EXT ".so"

#define G_VALID_IO_STATUS(s) (s == NET_OK || s == NET_MOREDATA)

class GTunnel {
	protected :
		
		string 		  m_name;
		void  		 *m_module;
		Configuration m_config;
		
		Listener	 *m_listener;
		
		static void *onIncomingConnection_dispatcher(void *ptr);
		static void *onTunnelingReady_dispatcher(void *ptr);
		
	public :
	
		GTunnel( char *modulename );
		~GTunnel();
		
		net_retcode_t start();
		
		void onIncomingConnection( Socket *client );
		void onTunnelingReady( Socket *source, Socket *destination );
};	

#endif