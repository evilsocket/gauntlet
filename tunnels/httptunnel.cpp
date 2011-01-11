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
#include <stdio.h>
#include <string.h>
#include <string>

using std::string;

unsigned char * mmfind( unsigned char *buffer, int size, unsigned char *needle, int nsize ){
	unsigned char *cptr1, 
  				  *eptr, 
  				  *cptr2, 
  				  *cptr3;
	int 		   i;

	if( nsize > size || nsize == 0 ){
    	return NULL;
	}
	
  	eptr = buffer + size - nsize;
 	--nsize;
	
	for( cptr1 = buffer; cptr1 <= eptr; ++cptr1 ){
		if( *cptr1 == *needle ){
      		cptr2 = cptr1;
      		++cptr2;
      		cptr3 = needle;
      		cptr3++;
      		for( i = nsize; i; --i, ++cptr2, ++cptr3 ){
				if( *cptr2 != *cptr3 ) break;
      		}
      		
      		if( !i ) return cptr1;
    	}
	}

	return NULL;
}

int mmreplace( unsigned char *buffer, int size, unsigned char *replace, int rsize, unsigned char *newbuffer, int nsize ){
	unsigned char *cptr, 
				  *cptr2, 
				  *cptr3, 
				  *eptr, 
				  *sptr;

	sptr = buffer;
	while( (cptr = mmfind( sptr, size - (sptr - buffer), replace, rsize )) ){
		cptr += rsize;
		if( nsize > rsize ){
			cptr2 = buffer + size - 1;
			cptr3 = cptr2 + nsize - rsize;
			while( cptr2 >= cptr ) *(cptr3--) = *(cptr2--);
		}
		else if( nsize < rsize){
			cptr3 = cptr;
			cptr2 = cptr3 + nsize - rsize;
			eptr  = buffer + size;
			while( cptr3 < eptr ) *(cptr2++) = *(cptr3++);
		}
	
		memcpy( cptr - rsize, newbuffer, nsize * sizeof(unsigned char) );
		size += nsize - rsize;
		sptr  = cptr - rsize + nsize;
	}

	return size;
}

extern "C" void init_handler(){
	// printf( "HTTP-TUNNEL PLUGIN INITIALIZED, THIS IS JUST AN EXAMPLE PLUGIN!\n" );
}

extern "C" void onincoming_payload_handler( int from, unsigned char *payload, int *psize ){	
	*psize = mmreplace( payload, *psize,
						(unsigned char *)"HTTP/1.1", strlen("HTTP/1.1"),
						(unsigned char *)"HTTP/1.0", strlen("HTTP/1.0") ); 
		
	
	*psize = mmreplace( payload, *psize, 
			  			(unsigned char *)"gzip, deflate", strlen("gzip, deflate"), 
			  			(unsigned char *)"deflate", 		strlen("deflate") );
			  
	printf( "%s\n------------------------------------------------------------------------------------------\n", payload );
}

