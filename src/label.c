/*
 * label.c
 *
 *  Created on: 2015/03/09
 *      Author: akira
 */


//#include <_mingw.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "mem.h"

int32_t __nis_hash (const void *keyarg, register size_t len);

/*
 * line number of input
 */
extern int line;

static size_t max_length_of_label = 32 ;
static size_t max_size_of_label   = 1024*1024;
static unsigned int rnd2_loop_l = 4 ;

static unsigned int label_used  = 0 ;
static unsigned int label_over  = 0 ;

char * label_table ;
int  * label_last  ;

/*
 * initalizer
 */
void label_init(size_t len, size_t size,unsigned int rnd2){
	printf("label.c : label_init\n");
	max_length_of_label = len ;
	max_size_of_label   = size ;
	rnd2_loop_l         = rnd2 ;

	label_table = my_mem_alloc(max_length_of_label * max_size_of_label,"label_table" );
	label_last  = my_mem_alloc(max_size_of_label * sizeof(*label_last),"label_last");
}

inline int label_hash_raw(void *items,int size) {
	printf("label.c : label_hash_raw\n");
	int ret = __nis_hash(items,size);
	if (ret == 0 ) ret =1 ; else if (ret<0) ret *= -1 ;
	return ret % max_size_of_label ;
}

inline char * point_of( int index ){
	printf("label.c : * point_of\n");
	return label_table + max_length_of_label * index ;
}

char * label_stat() {
	printf("label.c : * label_stat\n");
	static char buf[128];
	snprintf(buf,128, "labe[used:%d,over:%d]", label_used, label_over);
	return buf;
}

unsigned int get_label_index_of(char * items){
	printf("label.c : get_label_index_of\n");
	unsigned int index ,idx, min ;
	int diff;

	index = label_hash_raw(items,max_length_of_label);
	min   = index ;
	diff  = line - label_last[index];

	for(idx=0;idx<rnd2_loop_l; idx++){
		if ( strncmp(items,point_of(index),max_length_of_label)==0){
			label_last[index] = line ;
			return index ;
		}
		if ( * point_of(index) == '\0'){
			// new entry
			label_used ++;
			memcpy(point_of( index ) ,items, max_length_of_label ) ;
			label_last[index] = line ;
			return index ;
		}
		if ( diff > line - label_last[index])
			min = index ;
		index = label_hash_raw(&index,sizeof(index));
	}
	// overflow
	label_over ++ ;
	index = min ;
	memcpy(point_of( index ) ,items, max_length_of_label ) ;
	label_last[index] = line ;
	return  index;
}



//int get_label_index_of(char *label){
//	int index = label_hash( label );
//	if ( strncmp(label,point_of(index), max_length_of_label -1 ) != 0 )
//		fprintf(stderr,"label: SHOUTOTSU %s  %s \n",label,point_of(index));
//	memcpy(point_of( index ) ,label, max_length_of_label ) ;
//
//	return index ;
//}

char * get_label_by(int index){
	printf("label.c : * get_label_by\n");
	return point_of(index) ;
}
