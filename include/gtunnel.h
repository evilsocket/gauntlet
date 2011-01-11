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

typedef void (*g_init_handler_t)();
typedef void (*g_onincoming_connection_handler_t)( int );
typedef void (*g_ontunnelingready_handler_t)( int, int );
typedef void (*g_onincoming_payload_handler_t)( int, unsigned char *, int *);
typedef void (*g_onoutgoing_payload_handler_t)( int, unsigned char *, int *);
typedef void (*g_dispose_handler_t)();

class GTunnel {
	protected :
		
		string 		  m_name;
		void  		 *m_module;
		Configuration m_config;
		
		Listener	 *m_listener;
		
		static void *onIncomingConnection_dispatcher(void *ptr);
		static void *onTunnelingReady_dispatcher(void *ptr);
		
		g_init_handler_t				  m_init_handler;
		g_onincoming_connection_handler_t m_onincoming_connection_handler;
		g_ontunnelingready_handler_t	  m_ontunnelingready_handler;
		g_onincoming_payload_handler_t	  m_onincoming_payload_handler;
		g_onoutgoing_payload_handler_t	  m_onoutgoing_payload_handler;
		g_dispose_handler_t				  m_dispose_handler;
		
	public :
	
		GTunnel( char *modulename );
		~GTunnel();
		
		net_retcode_t start();
		
		void onIncomingConnection( Socket *client );
		void onTunnelingReady( Socket *source, Socket *destination );
};	

#endif