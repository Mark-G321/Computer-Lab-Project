/*
 * compilationHeader.h
 *This header contains all the relevant information for the defined language
 *  Created on: Jan 28, 2021
 *      Author: user
 */

#ifndef COMPILATIONHEADER_H_
#define COMPILATIONHEADER_H_

/*definition:*/
#define WORD 12
#define LINE 80
#define REGS 8
#define ILLEGALS 28
#define COMMANDS 16
#define MAXINT 2047
#define MININT -2048
#define MAXLABEL 31



/*command prototypes:*/
void updateData(int);
void firstPass(FILE*);
void secondPass(FILE *,int);
int isLegal(char*);
int labelLegality(char *);


/*command type definition*/
struct command{
	char name[5];
	int op;
	int funct;
	int param_num;
	/*parameter types where:0={no param},1={1},2={1,2},3={1,3},4={0,1,3}*/
	int src_type;
	int des_type;

};

extern int IC;
extern int DC;
extern int LC;
extern int SC;

#endif /* COMPILATIONHEADER_H_ */
