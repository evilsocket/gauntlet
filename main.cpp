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

#define MAIN_BACKLOG	 25
#define SERVER_ADDRESS   "www.google.com"
#define SERVER_PORT      80
#define READ_BUFFER_SIZE 0xFF

typedef struct {
	Socket *source;
	Socket *destination;
}
g_socket_tunnel_t;

void *socket_tunnel_function(void *ptr){
	g_socket_tunnel_t *tunnel = (g_socket_tunnel_t *)ptr;
	unsigned char      c;
	int  			   read, 
					   written, 
					   status;
	
	g_log_debug( "Inside socket tunnel [%s-> %s] .\n", tunnel->source->getAddress(), tunnel->destination->getAddress() );
	
	while( tunnel->source->read( &c, 1, &read ) == NET_OK ){
		if( read ){			
			g_log_debug( "Tunneling %d byte from %s to %s .\n", read, tunnel->source->getAddress(), tunnel->destination->getAddress() );
			
			tunnel->destination->write( &c, read, &written );
		}
	}
	
	return NULL;
}

void *server_acceptor(void *ptr){
	Socket 	      server_socket;
	Socket 	     *client  = (Socket *)ptr;
	int			  status;
	
	g_socket_tunnel_t client_tunnel, 
					  server_tunnel;
	Thread		  	  client_tunnel_thread(socket_tunnel_function),
					  server_tunnel_thread(socket_tunnel_function);
		
	if( (status = server_socket.connect( SERVER_ADDRESS, SERVER_PORT )) != NET_OK ){
		g_log_error( "Error %d connecting to real server %s:%d !\n", status, SERVER_ADDRESS, SERVER_PORT );
		
		delete client;
		return NULL;
	}
	
	g_log_info( "Created connection to %s:%d .\n", SERVER_ADDRESS, SERVER_PORT );
	
	client_tunnel.source      = client;
	client_tunnel.destination = &server_socket;
	
	server_tunnel.source 	  = &server_socket;
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
	g_init_log_level( LOG_DEBUG );
	
	Listener l( server_acceptor, 25, 10000 );
	
	l.start();

	return 0;
}