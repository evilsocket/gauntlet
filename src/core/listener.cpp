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
#include "listener.h"

Listener::Listener( threaded_function_t acceptor, unsigned long backlog, unsigned short port ) : Socket(), m_backlog(backlog), m_acceptor(acceptor), m_custom(NULL) {
	m_port = port;
}

void Listener::setCustomArg( void *args ){
	g_log_debug( "Setting custom argument %p for listener at %p .\n", args, this );
	m_custom = args;
}

net_retcode_t Listener::start(){
   struct sockaddr_in servaddr;
   struct sockaddr_in clientaddr;
   int				  client,
   					  addrlen = sizeof(clientaddr);
   Socket			 *csocket;
   Thread			 *cthread;
   
   memset(	&servaddr,   0, sizeof(servaddr) );
   memset(	&clientaddr, 0, addrlen );

   	m_sd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( !m_sd ){
    	g_log_error( "Could not create listener socket ([%d] %s).\n", errno, strerror(errno) );
        return NET_FAILED;
    }

	servaddr.sin_family 	 = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port 		 = htons(m_port);
	
	int opt = 1;
    if( setsockopt( m_sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt) ) == -1 ){
		g_log_error( "Could not set SO_REUSEADDR option ([%d] %s).\n", errno, strerror(errno) );
        return NET_FAILED;
    }
    // prevents broken pipe error during acccept
    if( setsockopt( m_sd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof(opt) ) == -1 ){
    	g_log_error( "Could not set SO_NOSIGPIPE option ([%d] %s).\n", errno, strerror(errno) );
        return NET_FAILED;
    }
   
	if( bind( m_sd, (sockaddr *)&servaddr, sizeof(servaddr) ) == -1 ){
		g_log_error( "Could not bind listener ([%d] %s).\n", errno, strerror(errno) );
        return NET_FAILED;
	}
	
	if( listen( m_sd, m_backlog ) == -1 ){
		g_log_error( "Could not start listener ([%d] %s).\n", errno, strerror(errno) );
        return NET_FAILED;
	}

    while( 1 ){    	
    	if( (client = accept( m_sd, (sockaddr *)&clientaddr, (socklen_t *)&addrlen )) != -1 ){
    		csocket = new Socket(client);
    		cthread = new Thread(m_acceptor);
    		
    		csocket->setAddress( clientaddr.sin_addr.s_addr );
    		csocket->setNonBlocking();
    		
    		g_log_info( "Accepted new connection from %s .\n", csocket->getAddress() );
    		
    		if( m_custom == NULL ){
				cthread->start( csocket );
			}
			else{
				g_listener_custom_args_t *args = (g_listener_custom_args_t *)calloc( 1, sizeof(g_listener_custom_args_t) );
				
				args->client = csocket;
				args->custom = m_custom;
				
				cthread->start( args );
			}	
			
			g_log_debug( "Client acceptor started .\n" );
    	}
    	else{
    		g_log_warning( "Error while accepting incoming connection ([%d] %s).\n", errno, strerror(errno) );
    	}
    }
   	
   	return NET_OK;
}