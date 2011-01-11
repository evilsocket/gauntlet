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
#include "config.h"

Configuration::Configuration(){

}

Configuration::~Configuration(){
	clear();
}

void Configuration::trim_line( char *linebuffer ){
	char trimmed[0xFF] = {0},
		 *p_src = linebuffer,
		 *p_dst = trimmed;

	// left trim
	while( isspace(*p_src) ) ++p_src;

	while( *p_src ){
		*p_dst = *p_src;
		++p_src;
		++p_dst;
	}

	// right trim
	p_dst--;
	while( isspace(*p_dst) ){
		*p_dst = 0x00;
		--p_dst;
	}

	strncpy( linebuffer, trimmed, 0xFF );
}

void Configuration::clear(){
	m_map.clear();
}

int Configuration::load( const char *filename ){
	FILE *fp;
	char  linebuffer[0xFF] = {0},
		  namebuffer[0xFF] = {0},
		  confbuffer[0xFF] = {0},
		 *p;
	int	  lineno = 1;
	
	clear();
	
	fp = fopen( filename, "rt" );
	if( !fp ){
		g_log_error( "Configuration file '%s' not found .\n", filename );
		return -1;
	}
	
	while( fgets( linebuffer, 0xFF, fp ) != NULL && !feof(fp) ){
		trim_line(linebuffer);
		// skip empty lines and comments
		if( linebuffer[0] != 0x00 && linebuffer[0] != '#' ){
			p = strpbrk( linebuffer, " \t" );
			// check for a valid separator
			if( p == NULL ){
				fclose(fp);
				g_log_error( "Could not find name-value separator on line %d of '%s' .\n", lineno, filename );
				return -1;
			}
			
			*p++ = 0x00;
			
			strncpy( namebuffer, linebuffer, 0xFF );
			strncpy( confbuffer, p, 0xFF );
			
			trim_line( namebuffer );
			trim_line( confbuffer );
			
			// g_log_debug( "[%s:%d][%s] : '%s' .\n", filename, lineno, namebuffer, confbuffer );
			
			m_map[namebuffer] = confbuffer;
		}
		++lineno;
		
		memset( linebuffer, 0x00, 0xFF );
		memset( namebuffer, 0x00, 0xFF );
		memset( confbuffer, 0x00, 0xFF );
	}	
	
	fclose(fp);
	return 0;
}

char *Configuration::get( char *name ){
	map<string,string>::iterator i;
	
	i = m_map.find(name);
	
	if( i == m_map.end()  ){
		g_log_warning( "Configuration value for '%s' not found .\n", name );
		return NULL;
	}
	else{
		return (char *)(*i).second.c_str();
	}
}

long Configuration::getLong( char *name, long defvalue /* = -1 */ ){
	map<string,string>::iterator i;
	
	i = m_map.find(name);
	
	if( i == m_map.end()  ){
		g_log_warning( "Configuration value for '%s' not found .\n", name );
		return defvalue;
	}
	else{
		return atol( (*i).second.c_str() );
	}
}

void Configuration::set( char *name, char *value ){
	m_map[name] = value;
}
