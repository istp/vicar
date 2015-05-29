/*
 * mem.h
 *
 *  Created on: 2015/03/09
 *      Author: akira
 */

#ifndef MEM_H_
#define MEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void * my_mem_alloc(size_t size, char * msg){
	void * res ;
	if (NULL==(res=malloc(size))) {
		fprintf( stderr, "%s: malloc() failed\n",msg );
		exit(EXIT_FAILURE);
	}
	memset(res,0,size);
	return res;
}

#endif /* MEM_H_ */
