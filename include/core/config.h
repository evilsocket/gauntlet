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
#ifndef __CONFIG_H
#	define __CONFIG_H

#include "log.h"
#include <ctype.h>
#include <string.h>
#include <map>
#include <vector>
#include <string>

using std::map;
using std::vector;
using std::string;

class Configuration {
	protected :
		map<string,string> m_map;
		
		
		void trim_line( char *linebuffer );
		
	public :
	
		Configuration();
		~Configuration();
		
		void clear();
		
		int load( const char *filename );
		
		char *get( char *name );
		long  getLong( char *name, long defvalue = -1 );
		void  getMultiple( char *name, vector<string>& values );
		
		void  set( char *name, char *value );
};

#endif
