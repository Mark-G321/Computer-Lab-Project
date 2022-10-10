/*
 * compailer.c
 *	This program compiles assembly files as described in mmn 14
 *  Created on: Jan 28, 2021
 *  Author: Mark Gurevich
*/

#include "header.h"
#include "compilationHeader.h"
#include "Tables.h"

int IC=100; /*instruction counter*/
int DC=0;	/*data counter*/
int LC=0;	/*label counter*/
int SC=0; 	/*line counter in original file*/

void compile(FILE *fp){
	int ICF;
	/*counters for compilation*/
	IC=100;
	DC=0;
	LC=0;
	SC=0;
	firstPass(fp);/*starts the first pass on the file*/
	if ((errorStatus(0))==TRUE){/*if finished first pass without errors continue compiling*/
		return;
	}
	ICF=IC;
	updateData(ICF);/*updates the data addresses*/
	secondPass(fp,ICF);/*starts the second pass on the file*/
	return;
}


void updateData(int ICF){
	labelobj *l;
	l=lfirst;
	if(l==NULL){
		return;
	}
	while(l->next!=NULL){
		if(l->attribute[0]=='s'||l->attribute[0]=='d')
			l->address+=ICF;

		l=l->next;
	}
	if(l->attribute[0]=='s'||l->attribute[0]=='d')
		l->address+=ICF;

	return;
}


/*this function returns whether a given string is a system word and thus illegal*/
int isLegal(char* input){
	int i;
	char *illegal[ILLEGALS]={"r0","r1","r2","r3","r4","r5","r6","r7",".string",".data",".entry",
						".extern","mov","cmp","add","sub","lea","clr","not",
						"inc","dec","jmp","bne","jsr","red","prn","rts","stop",};
	for (i=0;i<ILLEGALS;i++){
		if(!(strcmp(input,illegal[i]))){
			return FALSE;/*meaning passed string is illegal*/
		}
	}
	return TRUE; /*the passed string is legal*/


}
/*this function checks whether a passed label name is legal according to mmn14 definitions.*/
int labelLegality(char *name){
	int i;

	if(strlen(name)>MAXLABEL)/*label can be only 31 characters long*/
		return FALSE;
	if(isLegal(name)==FALSE){
		return FALSE;
	}
	if(!(isalpha(name[0])))/*first letter has to be alphabetical*/
		return FALSE;
	for(i=1;i<strlen(name);i++)/*check if the rest of the label is alphanumeric*/
		if((isalpha(name[i])==FALSE)&&(isdigit(name[i])==FALSE))
			return FALSE;
	return TRUE;
}
