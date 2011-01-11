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
#ifndef __THREAD_H
#	define __THREAD_H

#include "log.h"
#include <pthread.h>

typedef void *(* threaded_function_t)(void *);

class Thread {
	protected :
		pthread_t 			m_handle;
		void	 		   *m_return;
		threaded_function_t m_function;
		
	public :
	
		Thread( threaded_function_t function );
		
		int start( void *args );
		void *stop();
		  
		void *join();
		int detach();
};

#endif
