/*
 * errors.c is a file that handles errors during compilation.
 *
 *  Created on: Mar 7, 2021
 *      Author: Mark Gurevich
 */
#include "header.h"
/*this function handles error outputs.*/
void errorHandle(int location,char type){
	errorStatus(TRUE);

	switch (type){
		case 'l':
			printf("Assembly error in line %d. Error: Line length exceeding expected length.\n",location);
			break;
		case 's':
			printf("Assembly error in line %d. Error: Syntax error.\n",location);
			break;
		case ':':
			printf("Assembly error in line %d. Error: Missing \":\" after label definition.\n",location);
			break;
		case 'u':
			printf("Assembly error in line %d. Error: Unrecognized label name.\n",location);
			break;
		case 'e':
			printf("Assembly error in line %d. Error: Extraneous text detected.\n",location);
			break;
		case 'b':
			printf("Assembly error in line %d. Error: Label cannot be both external and entry.\n",location);
			break;
		case 'p':
			printf("Assembly error in line %d. Error: Operand type doesn't match types legal for command.\n",location);
			break;
		case 'c':
			printf("Assembly error in line %d. Error: Unidentified command.\n",location);
			break;
		case 'n':
			printf("Assembly error in line %d. Error: Number of operands don't match command requirements.\n",location);
			break;
		case 'w':
			printf("Assembly error in line %d. Error: Illegal number entered.\n",location);
			break;
		case 'd':
			printf("Assembly error in line %d. Error: Illegal data declaration attempted.\n",location);
			break;
		case 'i':
			printf("Assembly error in line %d. Error: Illegal label name.\n",location);
			break;
		case 'm':
			printf("Assembly error in line %d. Error: Multiple declaration of label detected.\n",location);
			break;
		case 'r':
			printf("Assembly error. Error: Cannot use relative operand types with external labels.\n");
			break;
		case 'o':
			printf("Assembly error in line %d. Error: Overflow detected.\n",location);
			break;
		case 'a':
			printf("Assembly error in line %d. Error: Unrecognized directive.\n",location);
			break;
		case ';':
			printf("Assembly error in line %d. Error: Illegal comment attempt.\n",location);
			break;
		case 'h':
			printf("Assembly error in line %d. Error: Illegal character in string.\n",location);
			break;
	}
	return;
}

/*this function returns the current error status, if function prameter is different then 0
 * also updates the error status*/
int errorStatus(int param){
	static int status=FALSE;

	if(param==1)/*set the status to TRUE*/
		status=TRUE;
	if (param==-1)/*resets the status*/
		status=FALSE;

	return status;
}
