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
#include "socket.h"
#include "thread.h"
#include "listener.h"
#include "config.h"

#define G_VALID_IO_STATUS(s) (s == NET_OK || s == NET_MOREDATA)

typedef struct {
	Socket *source;
	Socket *destination;
}
g_socket_tunnel_t;

static Configuration config;

void *socket_tunnel_function(void *ptr){
	g_socket_tunnel_t *tunnel = (g_socket_tunnel_t *)ptr;
	long			   bsize   = config.getLong( "ReadBufferSize", 0xFF );
	unsigned char      *buffer = (unsigned char *)calloc( bsize, 1 );
	int  			   read, 
					   written, 
					   status = NET_OK;
	
	g_log_debug( "Inside socket tunnel [%s-> %s] .\n", tunnel->source->getAddress(), tunnel->destination->getAddress() );
	
	while( G_VALID_IO_STATUS(status) ){
		status = tunnel->source->read( buffer, bsize, &read );
		if( G_VALID_IO_STATUS(status) ){
			if( read > 0 ){				
				g_log_debug( "Tunneling %d byte(s) from %s to %s .\n", read, tunnel->source->getAddress(), tunnel->destination->getAddress() );
	
				tunnel->destination->write( buffer, read, &written );
			}
		}
	}
	
	free(buffer);
	
	return NULL;
}

void *server_acceptor(void *ptr){
	Socket 	       tunnel_socket;
	Socket 	      *client  = (Socket *)ptr;
	int			   status;
	char		  *tunnel_address = config.get("TunnelAddress");
	unsigned short tunnel_port	  = config.getLong("TunnelPort"); 
	
	g_socket_tunnel_t client_tunnel, 
					  server_tunnel;
	Thread		  	  client_tunnel_thread(socket_tunnel_function),
					  server_tunnel_thread(socket_tunnel_function);
		
	if( (status = tunnel_socket.connect( tunnel_address, tunnel_port )) != NET_OK ){
		g_log_error( "Error %d connecting to real server %s:%d !\n", status, tunnel_address, tunnel_port );
		
		delete client;
		return NULL;
	}
	
	g_log_info( "Created connection to %s:%d .\n", tunnel_address, tunnel_port );
	
	client_tunnel.source      = client;
	client_tunnel.destination = &tunnel_socket;
	
	server_tunnel.source 	  = &tunnel_socket;
	server_tunnel.destination = client;
	
	client_tunnel_thread.start( &client_tunnel );
	server_tunnel_thread.start( &server_tunnel );
	
	g_log_info( "I/O tunnels started .\n" );
	
	server_tunnel_thread.join();
	
	client_tunnel_thread.stop();

	g_log_info( "I/O tunnels closed .\n" );
	
	delete client;
	return NULL;
}

int main(int argc,char **argv)
{
	if( config.load( "gauntlet.conf" ) != 0 ){
		return -1;
	}
	
	g_init_log_level( (g_log_level_t)config.getLong( "LogLevel", 0 ) );
	
	g_log_info( "Configuration succesfully loaded .\n" );
	
	Listener l( server_acceptor, 
				config.getLong( "ServerBacklog" ),
				config.getLong( "ServerPort" ) );
	
	l.start();

	return 0;
}