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

Socket::Socket() : m_sd(-1), m_timeout(10000), m_polltime(100), m_port(0), m_address(0) {
	memset( m_hostname, 0x00, 0xFF );
	memset( m_ipaddress, 0x00, 0xFF );
}

Socket::Socket( int sd ) : m_sd(sd), m_timeout(10000), m_polltime(100), m_port(0), m_address(0) {
	memset( m_hostname, 0x00, 0xFF );
	memset( m_ipaddress, 0x00, 0xFF );
}

Socket::Socket( char *hostname, unsigned short port ) : m_sd(-1), m_timeout(10000), m_polltime(100), m_port(0), m_address(0) {
	memset( m_hostname, 0x00, 0xFF );
	memset( m_ipaddress, 0x00, 0xFF );
	
	strncpy( m_hostname, hostname, 0xFF );
	
	m_port = port;
	
	struct hostent *host;

	// resolve hostname
	host = gethostbyname(m_hostname);
	if( host ){
		setAddress( *(unsigned long *)host->h_addr_list[0] );
	
		g_log_debug( "Hostname '%s' resolved to %s .\n", m_hostname, m_ipaddress );
	}
	else{
		g_log_error( "Could not resolve hostname '%s' .\n", m_hostname );
	}
}

Socket::~Socket(){
	::close( m_sd );
}

void Socket::setAddress( unsigned long address ){
	struct in_addr in_address;

	// get unsigned long repr of ip
	memcpy( &m_address, &address, sizeof(address) );
		
	// get string repr of ip
	in_address.s_addr = m_address;
	strncpy( m_ipaddress, inet_ntoa(in_address), 0xFF );
}

char *Socket::getAddress(){
	return m_ipaddress;
}

net_retcode_t Socket::setNonBlocking(){
    if( fcntl( m_sd, F_SETFL, fcntl(m_sd,F_GETFD) | O_NONBLOCK ) == -1 ){
    	g_log_error( "Could not set TCP socket to non blocking mode ([%d] %s).\n", errno, strerror(errno) );
        return NET_FAILED;
    }
    else{
    	return NET_OK;
    }
}

net_retcode_t Socket::connect( char *hostname, unsigned short port ) {
	m_sd 	   = -1; 
	m_timeout  = 10000;
	m_polltime = 100; 
	
	memset( m_hostname, 0x00, 0xFF );
	memset( m_ipaddress, 0x00, 0xFF );
	
	strncpy( m_hostname, hostname, 0xFF );
	
	m_port = port;
	
	struct hostent *host;

	// resolve hostname
	host = gethostbyname(m_hostname);
	if( host ){
		setAddress( *(unsigned long *)host->h_addr_list[0] );
	
		g_log_debug( "Hostname '%s' resolved to %s .\n", m_hostname, m_ipaddress );
	}
	else{
		g_log_error( "Could not resolve hostname '%s' .\n", m_hostname );
	}
	 
	return connect();
}

net_retcode_t Socket::connect(){
	if( m_address == 0 ){
		return NET_NOADDR;
	}	
	
	m_sd = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( !m_sd ){
    	g_log_error( "Could not create TCP socket ([%d] %s).\n", errno, strerror(errno) );
        return NET_FAILED;
    }
    
    struct sockaddr_in sin;
    
    memcpy( &sin.sin_addr, &m_address, sizeof(m_address) );
	
	sin.sin_family = AF_INET;
	sin.sin_port   = htons(m_port);
	
	// set socket to non-blocking mode
    if( setNonBlocking() != NET_OK ){
    	return NET_FAILED;
    }
    
	if( ::connect( m_sd, (struct sockaddr *)&sin, sizeof(sin) ) < 0 ){
		if( errno == EINPROGRESS ){
			int ret;
			do  {
				struct timeval tv;
				fd_set set;

				tv.tv_sec  = (int)m_timeout;
				tv.tv_usec = (int)((m_timeout*1000) - (double) (tv.tv_sec*1000));
				FD_ZERO(&set);
				FD_SET( m_sd, &set );

				if( (ret = select( m_sd + 1, NULL, &set, NULL, &tv ) ) < 0 ) {
					if( errno == EINTR ){
						g_log_error( "Error while connecting ([%d] %s).\n", errno, strerror(errno) );
						return NET_FAILED;
					}
				} 
				else if( ret > 0 ){
					g_log_debug( "Connected to %s succesfully .\n", m_ipaddress );
					return NET_OK;
				}
				else {
					g_log_error( "Error while connecting ([%d] %s).\n", errno, strerror(errno) );
					return NET_FAILED;
				}
			} 
			while(1);
		}
		else{
			g_log_error( "Error while connecting ([%d] %s).\n", errno, strerror(errno) );
			return NET_FAILED;
		}	
	}
	
	g_log_debug( "Connected to %s succesfully .\n", m_ipaddress );
	return NET_OK;
}

net_retcode_t Socket::read( unsigned char *buffer, unsigned int size, int *pread ){
	if( m_address == 0 ){
		return NET_NOADDR;
	}	
	
    unsigned char *pbuffer   = (unsigned char *)buffer;
    unsigned int   remaining = size,
    			   timeout   = 0,
    			   timedout  = 0;
    int 		   read     = 0,
				   error    = 0,
    			   status   = 0,
    			   complete = 0;
    
    struct timeval t;
    long timeout_sec  = (long)((m_polltime*1000) / 100000);
    long timeout_usec = (long)((m_polltime*1000) % 100000);

	*pread = 0;

    while( remaining && !timedout && !complete ){
        FD_ZERO( &m_fdset );
        FD_SET( m_sd, &m_fdset );
        
        t.tv_sec  = timeout_sec;
        t.tv_usec = timeout_usec;
        complete  = -1;

        status = select( m_sd + 1, &m_fdset, (fd_set *)0, (fd_set *)0, &t );
        switch(status){
            case -1:
                g_log_error( "Error during select ([%d] %s).\n", errno, strerror(errno) );
				return NET_FAILED;
				
            case 0 :
                if( *pread ){
                    complete = ((*pread == size) ? 1 : 0);
                }
               
               	timeout += m_polltime;
				if( timeout >= m_timeout ){
					timedout = 1;
					
					// undefined status
					if( complete == -1 ){
						g_log_error( "Read timed out ([%d] %s).\n", errno, strerror(errno) );
						return NET_TIMEDOUT;
					}
					// uncomplete request
					else if( complete == 0 ){
						g_log_debug( "Still %d bytes left to read from socket .\n", remaining );
						return NET_MOREDATA;
					}
					// complete
					else {
						return NET_OK;
					}
				}
            	continue;
                
            default:
                if( !FD_ISSET( m_sd, &m_fdset ) ){
                    g_log_error( "FD not set ([%d] %s).\n", errno, strerror(errno) );
                    return NET_FAILED;
                }

                read = recv( m_sd, pbuffer, remaining, 0 );
                // client disconnection
                if( read < 0 && (errno == EPIPE || errno == ECONNRESET) ){
                	remaining = 0;
                	shutdown( m_sd, 0 );
                	
                	g_log_warning( "Client disconnected during read operation .\n" );
                	
                	return NET_CLIENTDISCONNECT;
                }
                // probably no more data to read
                else if( read == 0 && errno == EINPROGRESS ){
                	return NET_OK;
                }
                // operation in progress ^^
                else if( read > 0 || (read == 0 && (errno == EALREADY || errno == EAGAIN)) ){                	
                	remaining -= read;
                	pbuffer	  += read;
                	*pread	  += read;
                	complete   = ((*pread == size) ? 1 : 0);
                }	
                // unhandled error
				else {
					remaining  = 0;
					shutdown( m_sd, 0 );
					
					g_log_error( "Error during read operation ([%d] %s).\n", errno, strerror(errno) );
					
					return NET_FAILED;
				}
            break;
        }
    }
    
    return NET_OK;
}

net_retcode_t Socket::write( unsigned char *buffer, unsigned int size, int *pwritten ){
	if( m_address == 0 ){
		return NET_NOADDR;
	}	
	
    unsigned char *pbuffer   = (unsigned char *)buffer;
    unsigned int   remaining = size,
    			   timeout   = 0,
    			   timedout  = 0;
    int 		   written  = 0,
				   error    = 0,
    			   status   = 0,
    			   complete = 0;
    
    struct timeval t;
    long timeout_sec  = (long)((m_polltime*1000) / 100000);
    long timeout_usec = (long)((m_polltime*1000) % 100000);

	*pwritten = 0;

    while( remaining && !timedout ){
        FD_ZERO( &m_fdset );
        FD_SET( m_sd, &m_fdset );
        
        t.tv_sec  = timeout_sec;
        t.tv_usec = timeout_usec;
        complete  = -1;

        status = select( m_sd + 1, (fd_set *)0, &m_fdset, (fd_set *)0, &t );
        switch(status){
            case -1:
                g_log_error( "Error during select ([%d] %s).\n", errno, strerror(errno) );
				return NET_FAILED;
				
            case 0 :
                if( *pwritten ){
                    complete = ((*pwritten == size) ? 1 : 0);
                }
               
               	timeout += m_polltime;
				if( timeout >= m_timeout ){
					timedout = 1;
					
					// undefined status
					if( complete == -1 ){
						g_log_error( "Write timed out ([%d] %s).\n", errno, strerror(errno) );
						return NET_TIMEDOUT;
					}
					// uncomplete request
					else if( complete == 0 ){
						g_log_debug( "Still %d bytes left to write to socket .\n", remaining );
						return NET_MOREDATA;
					}
					// complete
					else {
						return NET_OK;
					}
				}
            	continue;
                
            default:
                if( !FD_ISSET( m_sd, &m_fdset ) ){
                    g_log_error( "FD not set ([%d] %s).\n", errno, strerror(errno) );
                    return NET_FAILED;
                }

                written = send( m_sd, pbuffer, remaining, 0 );
                if( written < 0 && (errno == EPIPE || errno == ECONNRESET) ){
                	remaining = 0;
                	shutdown( m_sd, 0 );
                	
                	g_log_warning( "Client disconnected during write operation .\n" );
                	
                	return NET_CLIENTDISCONNECT;
                }
                else if( written == 0 && errno == EINPROGRESS ){
                	return NET_OK;
                }
                else if( written > 0 || (written == 0 && (errno == EALREADY || errno == EAGAIN)) ){
                	remaining -= written;
                	pbuffer	  += written;
                	*pwritten += written;
                	complete   = ((*pwritten == size) ? 1 : 0);
                }
				else {
					remaining  = 0;
					shutdown( m_sd, 0 );
					
					g_log_error( "Error during write operation ([%d] %s).\n", errno, strerror(errno) );
					
					return NET_FAILED;
				}
            break;
        }
    }
    
    return NET_OK;
}