/*
 * Tables.c this file manages the tables for code data and labels
 *
 *  Created on: Mar 5, 2021
 *      Author: Mark Gurevich
 */
#include "Tables.h"
#include "header.h"


/*pointers to start and end of tables:*/
labelobj *lfirst,*llast;
codeobj *cfirst,*clast;
dataobj *dfirst,*dlast;


 /*checks if passed label is already in symbol table
  * and returns pointer to it, return NULL if not found*/
 labelobj *searchTable(char*label){
 	labelobj *p;
 	extern int LC;
 	if(LC==0){
 		return NULL;
 	}
 	p=lfirst;
 	while(p!=NULL){
 		if(strcmp(p->name,label)==0){
 			return p;
 		}
 		p=p->next;
 	}
 	return NULL;

 }
 /*this function returns 0 if entry flag is off, else returns 1, passing -1 resets flag, and
  * passing 1 updates flag to be true */
 int entFlag(int check){
 	static int ent_flag=FALSE;
 	if (check==-1)
 		ent_flag=FALSE;
 	if(check==TRUE)
 		ent_flag=TRUE;
 	return ent_flag;
 }
 /*this function returns 0 if extern flag is off, else returns 1, passing -1 resets flag, and
  * passing 1 updates flag to be true */
 int extFlag(int check){
 	static int ext_flag=FALSE;
 	if (check==-1)
 		ext_flag=FALSE;
 	if(check==TRUE)
 		ext_flag=TRUE;
 	return ext_flag;
 }

/*this function resets the tables*/
 void reset(){
	 labelobj* ltemp;
	 codeobj * ctemp;
	 dataobj *dtemp;

	  extFlag(-1);
	  entFlag(-1);

	  while(lfirst){
		  free(lfirst->name);
		  ltemp=lfirst;
		  lfirst=lfirst->next;
		  free(ltemp);
	  }
	  while(cfirst){
		  free(cfirst->labelused);
		  ctemp=cfirst;
		  cfirst=cfirst->next;
		  free(ctemp);
	  }
	  while(dfirst){
	  	dtemp=dfirst;
	  	dfirst=dfirst->next;
	  	free(dtemp);
	  }
 }
