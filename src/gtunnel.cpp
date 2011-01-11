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
#include "gtunnel.h"

typedef struct {
	GTunnel *instance;
	Socket  *source;
	Socket  *destination;
}
g_socket_tunnel_t;

GTunnel::GTunnel( char *modulename ) : m_name(modulename), m_module(NULL) {
	string libname = m_name + G_MODULE_EXT,
		   cfgname = m_name + ".conf";
	
	m_init_handler 					= NULL;
	m_onincoming_connection_handler = NULL;
	m_ontunnelingready_handler		= NULL;
	m_onincoming_payload_handler	= NULL;
	m_onoutgoing_payload_handler	= NULL;
	m_dispose_handler				= NULL;
	
	m_module = dlopen( libname.c_str(), RTLD_NOW );
	if( !m_module ){
		g_log_error( "Could not load gtunnel module %s .\n", libname.c_str() );
		return;
	}
	
	m_init_handler 					= (g_init_handler_t					)dlsym( m_module, "init_handler" );
	m_onincoming_connection_handler = (g_onincoming_connection_handler_t)dlsym( m_module, "onincoming_connection_handler" );
	m_ontunnelingready_handler		= (g_ontunnelingready_handler_t     )dlsym( m_module, "ontunnelingready_handler" );
	m_onincoming_payload_handler	= (g_onincoming_payload_handler_t   )dlsym( m_module, "onincoming_payload_handler" );
	m_onoutgoing_payload_handler	= (g_onoutgoing_payload_handler_t   )dlsym( m_module, "onoutgoing_payload_handler" );
	m_dispose_handler				= (g_dispose_handler_t              )dlsym( m_module, "dispose_handler" );
	
	if( m_config.load( cfgname.c_str() ) != 0 ){
		g_log_error( "Could not load '%s' module configuration file '%s' .\n", m_name.c_str(), cfgname.c_str() );
		return;
	}
	
	m_listener = new Listener( GTunnel::onIncomingConnection_dispatcher,
							   m_config.getLong( "ServerBacklog" ),
							   m_config.getLong( "ServerPort" ) );
							   
	m_listener->setCustomArg(this);
	
	g_log_info( "Succesfully loaded module '%s' and its configuration .\n", m_name.c_str() );
	
	if( m_init_handler ) m_init_handler();
}

GTunnel::~GTunnel(){
	dlclose(m_module);
	delete m_listener;
	
	if( m_dispose_handler ) m_dispose_handler();
}

void *GTunnel::onIncomingConnection_dispatcher(void *ptr){
	g_listener_custom_args_t *args     = (g_listener_custom_args_t *)ptr;
	GTunnel 			     *instance = (GTunnel *)args->custom;
		
	instance->onIncomingConnection( args->client );
	
	delete args->client;
	
	free( args );
	
	return NULL;
}

void *GTunnel::onTunnelingReady_dispatcher(void *ptr){
	g_socket_tunnel_t *tunnel = (g_socket_tunnel_t *)ptr;
		
	tunnel->instance->onTunnelingReady( tunnel->source, tunnel->destination );
	
	return NULL;
}

net_retcode_t GTunnel::start(){
	if( m_listener ){
		return m_listener->start();
	}
	else{
		return NET_FAILED;
	}
}

void GTunnel::onIncomingConnection( Socket *client ){
	Socket 	       tunnel_socket;
	int			   status;
	char		  *tunnel_address = m_config.get("TunnelAddress");
	unsigned short tunnel_port	  = m_config.getLong("TunnelPort"); 
	
	g_socket_tunnel_t client_tunnel, 
					  server_tunnel;
	Thread		  	  client_tunnel_thread( GTunnel::onTunnelingReady_dispatcher ),
					  server_tunnel_thread( GTunnel::onTunnelingReady_dispatcher );
	
	if( m_onincoming_connection_handler ) m_onincoming_connection_handler( client->sd() );
	
	if( (status = tunnel_socket.connect( tunnel_address, tunnel_port )) != NET_OK ){
		g_log_error( "Error %d connecting to real server %s:%d !\n", status, tunnel_address, tunnel_port );
		return;
	}
	
	g_log_info( "Created connection to %s:%d .\n", tunnel_address, tunnel_port );
	
	client_tunnel.instance	  = this;
	client_tunnel.source      = client;
	client_tunnel.destination = &tunnel_socket;
	
	server_tunnel.instance	  = this;
	server_tunnel.source 	  = &tunnel_socket;
	server_tunnel.destination = client;
	
	client_tunnel_thread.start( &client_tunnel );
	server_tunnel_thread.start( &server_tunnel );
	
	g_log_info( "I/O tunnels started .\n" );
	
	server_tunnel_thread.join();
	
	client_tunnel_thread.stop();

	g_log_info( "I/O tunnels closed .\n" );
}

void GTunnel::onTunnelingReady( Socket *source, Socket *destination ){
	long		   bsize  = m_config.getLong( "ReadBufferSize", 0xFF );
	unsigned char *buffer = (unsigned char *)calloc( bsize, 1 );
	int  		   read, 
				   written, 
				   status = NET_OK;
	
	if( m_ontunnelingready_handler ) m_ontunnelingready_handler( source->sd(), destination->sd() );
	
	g_log_debug( "Inside socket tunnel [%s-> %s] .\n", source->getAddress(), destination->getAddress() );
	
	while( G_VALID_IO_STATUS(status) ){
		status = source->read( buffer, bsize, &read );
		if( G_VALID_IO_STATUS(status) ){
			if( read > 0 ){				
				if( m_onincoming_payload_handler ) m_onincoming_payload_handler( source->sd(), buffer, &read );
			
				g_log_debug( "Tunneling %d byte(s) from %s to %s .\n", read, source->getAddress(), destination->getAddress() );
	
				if( m_onoutgoing_payload_handler ) m_onoutgoing_payload_handler( destination->sd(), buffer, &read );
		
				destination->write( buffer, read, &written );
			}
		}
	}
	
	free(buffer);
}
