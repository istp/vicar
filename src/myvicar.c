/*
 ============================================================================
 Name        : myvicar.c
 Author      : Akira SATO
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mem.h"
#include "line.h"
#include "label.h"
#include "debug.h"

/*
 * parameter for calcurate
 */
size_t        item_size   = 5;
size_t        cache_limit = 1024 * 1024 ;
size_t        card_size   = 8;
size_t        item_length = 32 ;
unsigned int  rnd2_loop   = 4 ;
unsigned int  threshold   = 100 ;
unsigned int  par_stat    = 1000;
unsigned int  debug_level = 0 ;
size_t        time_len    = 0 ;


/*
 * array
 */
//int  * item_set ;
int  * line_items ;
int  * crnt_items ;

int  * cache_hash ;
int  * cache_count ;

int  * card_hash;
int  * card_count ;

char * card_string ;

unsigned int  * first;
unsigned int  * last ;
unsigned int  * lastidx ;

int    idx   = 0;

char * times ;

unsigned int    line  = 0;
unsigned int    used  = 0;
unsigned int    over  = 0;

char  *program_name ;

extern int __nis_hash();

void print_out();

inline int cached_item(int index, int pos ){
	return *(cache_hash + index * item_size + pos);
}

inline unsigned int pnis_hash(void *items, int size) {
    int i=__nis_hash(items, size);
    if (i==0) return 1; else if (i>0) return i ; else return -i; }

int hash2( int *items ) {
	unsigned int h, i, idx, j, mark;
	int      min ;
	h=pnis_hash(items, sizeof(items[0])*item_size);
	i=h%cache_limit;
	for (idx=0,min=i; idx<rnd2_loop; idx++) {
		for (j=0,mark=1; (j<item_size)&&mark; j++)
			mark=((items[j]==0)&&(cache_hash[j+i*item_size]<=0))||(cache_hash[j+i*item_size]==items[j]);
		if (mark) return i;
		if (cache_count[min]>cache_count[i]) min=i;
		h=pnis_hash( &h, 4 );
		i=h%cache_limit;
	}
	return(-min);
}


inline int get_card_count(int index,int pos, int index2){
	return *(card_count+(index*item_size+pos)*card_size+index2);
}

inline int get_card_hash(int index,int pos, int index2){
	return *(card_hash+(index*item_size+pos)*card_size+index2);
}


/*
 * 異なり値のためのテーブルをクリアする。
 */
void card_clear(int index){
	if(card_size>0){
		memset(card_hash +index*item_size*card_size,0,sizeof(*card_hash )*card_size*item_size);
		memset(card_count+index*item_size*card_size,0,sizeof(*card_count)*card_size*item_size);
	}
}

/*
 * 異なり値のための処理をする。b
 * index : item_set のエントリーのindex
 * pos   : item_set 内の異なり値を記録する位置
 * flag  : true ならエントリーを追加する。
 * 返値  : 異なり数が増えるときは1。それ以外は0。
 */
int card_store(int index,int pos,int flag){
	int i ;
	int ret = 0 ;
	if (line_items[pos] == 0) {
		sprintf(stderr,"ERROR:card_store:unmatch pos=%d : %8d:%-16s\n",pos,line_items[pos],get_label_by(line_items[pos]));
		print_out(index,"e");
		return 0 ; // BUG??
	}

	// 空きスペースの検索
	for(i=0;(i<card_size)&&(get_card_count(index,pos,i)>0);i++){
		if(get_card_hash(index,pos,i)==line_items[pos]){
			// すでにエントリがある。
			break;
		}else if(get_card_hash(index,pos,i)==0){
			// エントリにはない。
			if(!flag){
				fprintf(stderr,"ERROR:card_store:not find  pos=%d : %8d:%-16s\n",pos,line_items[pos],get_label_by(line_items[pos]));
				print_out(index,"e");
				return 0;
			}
			ret = 1 ; // エントリを追加すべき
		}
	}
	if( i>= card_size ){
		// over flow
		fprintf(stderr,"warnning:card_store:card can't find(overflow) pos=%d : %8d:%-16s\n",pos,line_items[pos],get_label_by(line_items[pos]));
		print_out(index,"w");
		if ( flag ){
			i = card_size -1 ;
			ret = 1 ;
		}
	}else if(get_card_hash(index,pos,i)==line_items[pos]){
		ret = 1 ;
	}else {
		ret = flag?1:0;
	}
	if( ret == 1){
		card_hash[(index*item_size+pos)*card_size+i] =line_items[pos];
		card_count[(index*item_size+pos)*card_size+i]=1;
	}else{
		card_count[(index*item_size+pos)*card_size+i]++;
		// カウント値の降順に並べ替える
		if((i!=0)&&get_card_count(index,pos,i-1)<get_card_count(index,pos,i)){
			int tmp_count = card_count[(index*item_size+pos)*card_size+i] ;
			int tmp_hash  = card_hash[(index*item_size+pos)*card_size+i];
			card_count[(index*item_size+pos)*card_size+i]   = card_count[(index*item_size+pos)*card_size+i-1];
			card_hash[(index*item_size+pos)*card_size+i]    = card_hash[(index*item_size+pos)*card_size+i-1];
			card_count[(index*item_size+pos)*card_size+i-1] = tmp_count ;
			card_hash[(index*item_size+pos)*card_size+i-1]  = tmp_hash ;
		}
	}
	return ret ;
}





char * get_stat_str(){
	static char buf[256];
	snprintf(buf,256,"hash[used:%d over:%d] ",used,over);
	return buf;
}


inline void set_entry(int index){
	memcpy(cache_hash+index*item_size,crnt_items,sizeof(*cache_hash)*item_size);
	cache_count[index] = 0 ;
	card_clear(index);
}

inline void reset_entry(index){
	int i ;
	for(i=0;i<item_size;i++){
		if ( cached_item(index,i)<0 )
			cache_hash[index*item_size+i] = 0;
	}
	cache_count[index] = 0 ;
	card_clear(index);
}

inline void first_entry(int index){
	first[index]      = line ;
	last[index]       = line ;
	memcpy(times+time_len*index,get_time(),time_len); // 時刻の記録
}

// テーブルにエントリを追加する。
int store(){
	int index = hash2(crnt_items);
	if (index <= 0 ){
		index *= -1 ;
		if( cache_count[index]!=0 ) {// 古いエントリを再利用
			print_out(index,'D');
			over ++;
		}else{
			used ++;
		}
		set_entry(index);
		first_entry(index);
	}else{
		if( cache_count[index]==0 ){
			first_entry(index);
		}
	}

	lastidx[idx++]=index;
	cache_count[index]++;
	return index ;
}

inline void cleanup_store(){
	int index ;
	for(--idx;idx>=0;idx--){
		index = lastidx[idx];
		last[index]=line ;
		if( cache_count[index] >= threshold){
			print_out(index,"S");
			reset_entry(index);
		}
	}
	idx=0;
}

inline void variety_check(int target,int subject,int pos){
	if ( (target <= 0) || (subject <=0) || (cache_count[subject] == 0) ) return ;
	if (is_for_debug(DEBUG_VARIETY)){
		debug_print_cache_hash(target,"v");
		debug_print_cache_hash(subject,"V");
	}

	int flag = ((first[target]==line) || (first[target]>last[subject]) || (last[target]<first[subject]));
	// * flag  : true ならエントリーを追加する。
	int ret  = card_store(target,pos,flag);
	*(cache_hash+item_size*target+pos) -= ret ;
}



int ViG(int pos){
	int i ; // loop counter
	int index = 0 ;
	if( pos != 0 ){
		index = store();
		if(is_for_debug(DEBUG_VIG)){
			debug_print_cache_hash(index,"G");
		}
		// 異なり数の計数
		for(i=0;i<pos;i++){
			if( cached_item(index,i) <= 0 ){
				crnt_items[i]=line_items[i];
				variety_check(index,hash2(crnt_items),i);
				crnt_items[i]=0;
			}
		}
		if(is_for_debug(DEBUG_VIG)){
			debug_print_cache_hash(index,"g");
		}
	}
	for(i=pos;i<item_size;i++){
		crnt_items[i] = line_items[i];
		int res = ViG(i+1);
		crnt_items[i] = 0;
		variety_check(index,res,i);
	}
	return index;
}

inline void action_item(int i){
	*(line_items+i) = get_label_index_of(get_item(i));
}

inline void action_line(){
	memset(crnt_items,0,sizeof(*crnt_items)*item_size);
	(void)ViG(0);
	cleanup_store();
}

void main_loop(){
	int num_of_items;
	while(!feof(stdin)){
		num_of_items = read_line();
		if (num_of_items <= 0 ) break ;
		line ++ ;
		if ( num_of_items !=  item_size ){
			fprintf(stderr,"LINE %d:NUMBER OF ITMES is expected %d, but %d.\n",line,(int)item_size,num_of_items);//(int) 5/19
		}else{
			size_t i ; // loop counter
			for(i=0;i<num_of_items;i++){
				action_item(i);// item 毎の処理
			}
			if(is_for_debug(DEBUG_INPUT))
				debug_print_crnt_itemset();
			action_line();// line 毎の処理
		}
		if ((par_stat != 0) && (line % par_stat == 0)) {
			fprintf(stderr,"STAT %d %s,%s\n",line,get_stat_str(),label_stat());
		}
	}

}

void usage(){
	fprintf(stderr,"Usage : %s [OPTIONS] [FILE] ....\n",program_name);
	fputs("\
\n\
-d num : debug level\n\
-c num : cache size\n\
-t num : thresh\n\
-i num : item sets\n\
-l num : rnd2s\n\
-n num : name string length\n\
-v num : vcard\n\
-p num : output stat per line\n\
",stderr);
	exit(EXIT_FAILURE);
}
/*

-m num : mthresh\n\
-o num : output\n\
-d num : debug\n\
*/

int main(int argc, char **argv) { extern char *optarg;
	extern char *optarg;
	extern int   optind;

	/* Name of input file.  May be "-".  */
	char const *infile;

	/* Index in argv to processed argument.  */
	int argind;


	int    i ;


	while((i=getopt(argc,argv,"d:c:t:i:l:n:p:v:T:h?")) != -1)
		switch ( i ) {
		case 'd':
			debug_level = abs(atoi(optarg));break;
		case 'c':
			cache_limit = abs(atoi(optarg))*1024;	break;
		case 't':
			threshold   = abs(atol(optarg));break;
		case 'i':
			item_size   = abs(atoi(optarg));break;
		case 'l':
			rnd2_loop   = abs(atoi(optarg));break;
		case 'n':
			item_length = abs(atoi(optarg));break;
		case 'p':
			par_stat    = abs(atoi(optarg));break;
		case 'v':
			card_size   = abs(atoi(optarg));break;
		case 'T':
			time_len    = abs(atoi(optarg));break;
		default:
			usage();
		}

	line = 0 ;
	label_init(item_length,cache_limit,rnd2_loop);

//	item_set   = my_mem_alloc(sizeof(*item_set   )*item_size,"item_set");
	line_items = my_mem_alloc(sizeof(*line_items )*item_size,"line_items");
	crnt_items = my_mem_alloc(sizeof(*crnt_items )*item_size,"crnt_items");

	cache_hash = my_mem_alloc(sizeof(*cache_hash )*item_size*cache_limit,"cache_hash");
	cache_count= my_mem_alloc(sizeof(*cache_count)*cache_limit,"cache_count");

	card_hash  = my_mem_alloc(sizeof(*card_hash )*cache_limit*item_size*card_size,"card_hash");
	card_count = my_mem_alloc(sizeof(*card_count)*cache_limit*item_size*card_size,"card_count");

	card_string = my_mem_alloc(sizeof(*card_string)*item_length,"card_string");

	first      = my_mem_alloc(sizeof(*first      )*cache_limit,"first");
	last       = my_mem_alloc(sizeof(*first      )*cache_limit,"last" );

	lastidx    = my_mem_alloc(sizeof(*lastidx    )*(1<<item_size),"lastidx");
	idx        = -1 ;

	times      = my_mem_alloc(sizeof(*times)      *cache_limit,"times");

	read_line_init(time_len, item_length,item_size );

	program_name = argv[0];


	infile = "-";
	argind = optind;


  // main loop
	do {
		if (argind < argc)
			infile = argv[argind];
		if (strcmp(infile,"-") == 0 ){
			freopen("","rb",stdin);
		}else{
			if(  freopen( infile,"rb",stdin) == NULL ) {
				fprintf(stderr,"can't open file : %s\n",infile);
				continue ;
			}
		}
		main_loop();

		if ( (strcmp(infile,"-")!=0) && fclose(stdin) != 0 ){
			fprintf(stderr,"can't close file : %s",infile);
		}
	} while( ++argind < argc );

printf("aaa");
	return (EXIT_SUCCESS);
}

