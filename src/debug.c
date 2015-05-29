/*
 * debug.c
 *
 *  Created on: 2015/04/14
 *      Author: akira
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "label.h"

extern char * card_string;
extern int    item_length;
extern int    card_size ;
extern int    item_size ;
extern int    line ;
extern int   *first;
extern int   *last ;
extern int   *cache_count;
extern int   *cache_hash;
extern int   *crnt_items;
extern int    get_card_count();
extern int    get_card_hash();
extern int    cached_item();


inline char * make_cardinarity_label(int index, int pos){
	int i ;
	memset(card_string,0,item_length);
	for(i=0;i<card_size;i++){
		int len = strlen(card_string) ;
		if (len+1>item_length) break ;
		snprintf(card_string+len,item_length-len-1,"%d:",get_card_count(index,pos,i));
	}
	return card_string;
}

#define W_STR    1
#define W_LINE   6
#define W_HASH   8
#define W_COUNT  6
#define W_LABEL  16

#define FMT_HEADER   "%*s%0*d:%*x:%*d:%*d:%*d"
#define FMT_HEADER_S "%*s%0*d:%*s:%*s:%*s:%*s"
#define FMT_HEADER_D "%*s%-*s %*s %*s %*s %*s"

#define FMT_ITEM     ":%*x:%-*s"
#define FMT_ITEM_S   ":%*d:%-*s"
#define FMT_ITEM_D   ":%*s:%-*s"

inline void debug_print_header(FILE *fd,int index, char *str){
	fprintf(fd, FMT_HEADER,
			W_STR,  str,
			W_LINE, line,
			W_HASH, index,
			W_COUNT,cache_count[index],
			W_COUNT,first[index],
			W_COUNT,last[index]);
}

inline void debug_print_header_simple(FILE *fd,char *str1,char *str2){
	fprintf(fd, FMT_HEADER_S,
			W_STR,  str1,
			W_LINE, line,
			W_HASH, str2,
			W_COUNT,"",
			W_COUNT,"",
			W_COUNT,"");
}

inline void debug_print_header_dummy(FILE *fd){
	fprintf(fd, FMT_HEADER_D,
			W_STR,  "",
			W_LINE, "",
			W_HASH, "",
			W_COUNT,"",
			W_COUNT,"",
			W_COUNT,"");
}


inline void debug_print_itemsets(FILE *fd,int *sets,int index ){
	int i ;
	for(i=0;i<item_size;i++)
		if(*(sets+i)<=0){
			fprintf(fd,FMT_ITEM_S,
					W_HASH, *(sets+i),
					W_LABEL,(index==0)?"(NA)":make_cardinarity_label(index,i));
		}else{
			fprintf(fd,FMT_ITEM,
					W_HASH, *(sets+i),
					W_LABEL,get_label_by(*(sets+i)));
		}
	fprintf(fd,"\n");
}

inline void debug_print_crnt_itemset(){
	debug_print_header_simple(stderr,"d","crnt");
	debug_print_itemsets(stderr,crnt_items,0);
}


inline void debug_print_cache_hash(int index,char *str){
    debug_print_header(stderr,index,str);
	debug_print_itemsets(stderr,cache_hash+item_size*index,0);
}

extern char * times ;
extern size_t time_len ;

inline void print_out(int index,char *str){
    debug_print_header(stdout,index,str);
    if(time_len != 0 ) printf(":%s",times+index*(time_len+1));
    debug_print_itemsets(stdout,cache_hash+item_size*index,index);

	int i,j ;
	for(j=0;j<card_size;j++){
		debug_print_header_dummy(stdout);
		for(i=0;i<item_size;i++)
			if((cached_item(index,i)<=0)&&(get_card_count(index,i,j)>0)){
				printf(FMT_ITEM,
						W_HASH,  get_card_hash(index,i,j),
						W_LABEL, get_label_by(get_card_hash(index,i,j)));
			}else{
				printf(FMT_ITEM_D,
						W_HASH,  "",
						W_LABEL, "");
			}
		printf("\n");
	}
}
