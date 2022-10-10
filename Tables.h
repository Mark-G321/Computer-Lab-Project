/*
 * Tables.h is a header files for tables information.
 *
 *  Created on: Mar 5, 2021
 *      Author: user
 */

#ifndef TABLES_H_
#define TABLES_H_
/*struct definition*/
typedef struct label{
	char *name;
	int address;
	char attribute[2];
	struct label *next;
}labelobj;

typedef struct code_element{
	int address;
	int code;
	char dependency;
	char *labelused;/*this is only used for identifying labels as parameters*/
	int labelLine;/*used along side labelused to remember the line at which it was used*/
	struct code_element *next;
}codeobj;

typedef struct data_element{
	int code;
	struct data_element *next;
}dataobj;
/*function prototypes*/
void reset();
labelobj* searchTable(char *);
int extFlag(int);
int entFlag(int);

extern labelobj *lfirst,*llast;
extern codeobj *cfirst,*clast;
extern dataobj *dfirst,*dlast;


#endif /* TABLES_H_ */
