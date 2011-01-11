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
#include "log.h"

static g_log_level_t __g_loglevel;
static FILE			*__g_loghandle = stdout;

void g_init_log_level( g_log_level_t level ){
	__g_loglevel = level;
}

void g_init_log_handle( FILE *handle ){
	__g_loghandle = handle;
}

void g_log_message( g_log_level_t level, const char *format, ... ){
	char 	buffer[0xFF] 	= {0},
			timestamp[0xFF] = {0},
		   *slevel;
	va_list ap;
	time_t 		rawtime;
  	struct tm * timeinfo;
	
	if( level >= __g_loglevel ){
		va_start( ap, format );
			vsnprintf( buffer, 0xFF, format, ap );
		va_end(ap);

		time( &rawtime );
  		timeinfo = localtime( &rawtime );

  		strftime( timestamp, 0xFF, "%d/%m/%Y %X", timeinfo );
		
		switch( level ){
			case LOG_DEBUG   : slevel = "DEBUG"; break;
			case LOG_WARNING : slevel = "WARNING"; break;
			case LOG_INFO    : slevel = "INFO"; break;
			case LOG_ERROR   : slevel = "ERROR"; break;
		}
		
		fprintf( __g_loghandle, "[%s] [%s] %s", timestamp, slevel, buffer );
    }
}
