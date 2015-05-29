/*
 * debug.h
 *
 *  Created on: 2015/05/05
 *      Author: akira
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG_INPUT   0x01
#define DEBUG_VIG     0x02
#define DEBUG_VARIETY 0x04

extern inline void debug_print_crnt_itemset();
extern  unsigned int debug_level ;
#define is_for_debug(NUM) (debug_level & (NUM))

#endif /* DEBUG_H_ */
