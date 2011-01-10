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
#include "thread.h"

Thread::Thread( threaded_function_t function ) : m_handle(0), m_return(NULL), m_function(function) {
	pthread_setcancelstate( PTHREAD_CANCEL_ENABLE, NULL );
	pthread_setcanceltype( PTHREAD_CANCEL_DEFERRED, NULL );
}

int Thread::start( void *args ){
	return pthread_create( &m_handle, NULL, m_function, args );
}

void *Thread::stop(){
	pthread_cancel(m_handle);
	
	return join();
}
		  
void *Thread::join(){
	pthread_join( m_handle, &m_return );
	
	return m_return;
}

int Thread::detach(){
	return pthread_detach(m_handle);
}