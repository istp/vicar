/*
 * label.h
 *
 *  Created on: 2015/03/18
 *      Author: akira
 */


#ifndef LABEL_H_
#define LABEL_H_

extern void label_init(size_t len, size_t size,unsigned int);

extern char * label_stat() ;

extern unsigned int get_label_index_of(char *);

extern char * get_label_by(int);

#endif /* LABEL_H_ */

