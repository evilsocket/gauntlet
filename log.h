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
#ifndef __LOG_H
#	define __LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

typedef enum {
	LOG_DEBUG   = 0,
	LOG_WARNING,
	LOG_INFO,
	LOG_ERROR
}
g_log_level_t;

void g_init_log_level( g_log_level_t level );
void g_init_log_handle( FILE *handle );

void g_log_message( g_log_level_t level, const char *format, ... );

#define g_log_debug( ... )	 g_log_message( LOG_DEBUG, __VA_ARGS__ )
#define g_log_warning( ... ) g_log_message( LOG_WARNING, __VA_ARGS__ )
#define g_log_info( ... ) 	 g_log_message( LOG_INFO, __VA_ARGS__ )
#define g_log_error( ... )   g_log_message( LOG_ERROR, __VA_ARGS__ )

#endif
