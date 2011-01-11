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
#include "gtunnel.h"

void *gtunnel_async_starter( void *p ){
	GTunnel *tunnel = (GTunnel *)p;
	
	tunnel->start();
	
	return NULL;
}	

int main(int argc,char **argv)
{
	Configuration  			 config;
	char		  			*modules_path;
	vector<string> 		     modules;
	vector<string>::iterator i;
	vector<GTunnel *> 		 tunnels;
	vector<Thread *>		 starters;
	int						 j;
	
	if( config.load( "gauntlet.conf" ) != 0 ){
		return -1;
	}
	
	g_init_log_level( (g_log_level_t)config.getLong( "LogLevel", 0 ) );
	
	g_log_info( "Configuration succesfully loaded .\n" );
	
	modules_path = config.get( "ModulesPath" );
	
	g_log_info( "Loading modules from '%s' ...\n", modules_path );
	
	config.getMultiple( "LoadModule", modules );
	
	for( i = modules.begin(); i != modules.end(); i++ ){
		g_log_info( "\tLoading '%s' (%s/%s%s) .\n", (*i).c_str(), modules_path, (*i).c_str(), G_MODULE_EXT );
		
		tunnels.push_back( new GTunnel( (char *)(*i).c_str() ) );
		starters.push_back( new Thread( gtunnel_async_starter ) );
	}

	for( j = 0; j < tunnels.size(); ++j ){
		starters[j]->start( tunnels[j] );
	}

	for( j = 0; j < tunnels.size(); ++j ){
		starters[j]->join();
	}
	
	return 0;
}