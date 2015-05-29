/*
 * line.c
 *
 *  Created on: 2015/03/08
 *      Author: akira
 */

#include <stdio.h>
#include <stdlib.h>

#include "mem.h"

static char * item_str;
static size_t item_len;
static size_t item_size;

static char * time_str;
static size_t time_len;


void read_line_init(size_t time_length, size_t item_length, size_t num_of_items ){
	printf("line.c : read_line_init\n");
	// prepare line_item
	item_len = item_length;
	item_size = num_of_items ;
	item_str = my_mem_alloc(item_len*item_size,"item_str");

	// prepare time_str
	time_len = time_length;
	time_str = my_mem_alloc(time_len+1,"time_str");
}

/*
 * get field
 * 空白、タブ、改行で文字列を区切って返す。
 * buf_len で指定された長さ以上であれば、buf_len -1 までの文字列をコピーして、その先は読み飛ばす。
 *
 * buf : 文字列を格納すべき場所へのポインタ
 * buf_len : buf が指し示す領域の大きさ
 * 返値：文字列の長さ、ただし改行で区切られた場合は、長さ * -1
 */
inline int get_field(char * buf,size_t buf_len){
	printf("label.c : get_field\n");
	int  c;
	int  i = 0  ; // loop counter
	memset(buf,0,buf_len);
	while( (c= getchar())!=EOF  && c == ' ');
	do{
		switch( c ){
		case '\r':
		case '\n':
			i *= -1 ;
			goto OUT_OF_LOOP;
			break;
		case '\t':
		case ' ':
			goto OUT_OF_LOOP;
			break;
		default:
			if ( i < buf_len -1 ) buf[i] = c;
			i ++ ;
			break;
		}
	}while( (c=getchar()) != EOF);
	OUT_OF_LOOP:
	return  ((c!=EOF)?i:-i);
}

extern int line ;

int read_line(){
	printf("label.c : read_line\n");

	size_t i ; // loop counter
	int  c ;
	int  ret ;
	char dummy_item;

	memset(item_str,0,item_len*item_size);
	memset(time_str,0,time_len);

	// read time_str
	i=0 ;
	while( (i< time_len) && (c=getchar())!=EOF ){
		*(time_str + i) = c ;
		i++;
	}
	if (i!= time_len)
		return -1;
	else
		*(time_str + i) = 0;

	i = -1 ;
	do{
		i++ ;
		if ( i < item_size ) {
			ret = get_field(item_str+(i*item_len),item_len);
			if( ((ret<0) ? -ret : ret) >item_len ){
				// 長すぎたとワーニングメッセージを出す。
				fprintf(stderr,"LINE %d:TOO LONG STRING (MAX %d): %s\n",
						line,(unsigned int)item_len,item_str+(i*item_len));
			}
		}else{
			ret = get_field(&dummy_item,1);
		}
	}while (ret > 0 );
	if (ret < 0) i ++ ;
	return i;

}


char * get_time(){
	printf("label.c : * get_time\n");
	return time_str;
}

char * get_item(int index){
	printf("label.c : * get_item : %d\n",index);
	return item_str+item_len*index;
}
