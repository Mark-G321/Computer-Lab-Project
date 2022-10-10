/*
 * second-pass.c Does the second pass according to mmn 14 algorithm
 *
 *  Created on: Mar 7, 2021
 *      Author: Mark Gurevich
 */
#include "header.h"
#include "compilationHeader.h"
#include "Tables.h"

void secondParse(char*);

/*this function performs the second pass on the file*/
void secondPass(FILE *fp,int ICF){
	char line[LINE];
	codeobj *co=cfirst;
	labelobj *l;
	clearerr(fp);/*clears EOF after first pass*/
	rewind(fp);
	SC=0;/*resets line counter after rewind*/
	while(!feof(fp)){
		if(!(fgets(line,81,(FILE*)fp))){/*if returned null or we reached EOF*/
			if(feof(fp)){/*if EOF*/
				break;
			}
			else{/*error occured*/
				printf("Error: Failed to read line %d",SC);
				continue;
			}
		}
		SC++;/*increase line counter after reading a line.*/
		if (line[strlen(line)-1]=='\n'){/*if received normal length line*/
			line[strlen(line)-1]='\0';
			secondParse(line);
		}
	}
	rewind(fp);
	SC=0;/*resets line counter after rewind*/
	while(co!=NULL){ /*completes missing label addresses:*/
		if (co->dependency!='?'){
			co=co->next;
			continue;
		}
		else if(co->labelused[0]=='%'){/*if label used as type 2 parameter*/
			if((l=searchTable(co->labelused+1))==NULL){/*if label isnt in table report error*/
				errorHandle(co->labelLine,'u');
				co=co->next;
				continue;
			}
			if (l->attribute[0]=='e'){/*if its an externl label*/
				errorHandle(co->labelLine,'r');/*cannot use relative parameters with external labels*/
				continue;
			}

			co->dependency='A';
			co->code=(l->address)-(co->address);/*calculates the distance to label*/
			co=co->next;
			continue;
		}
		else{/*label used as type 1 operand*/
			if(!(l=searchTable(co->labelused))){/*if label isnt in table report error*/
				errorHandle(co->labelLine,'u');
				co=co->next;
				continue;
			}
			if (l->attribute[0]=='e'){/*if its an external label*/
				co->dependency='E';
			}
			else
				co->dependency='R';

			co->code=(l->address);
			co=co->next;
			continue;
		}
	}
	return;
}

/*this function parses the .entry lines*/
void secondParse(char *line){
	char word[LINE];
	char label[LINE];
	char extra[LINE];
	labelobj *l;

	/*if its not an entry line it has been taken care of in the first pass*/
	if(!(strstr(line,".entry"))){
		return;
	}
	sscanf(line," %s ",word);/*read the first word*/

	/*if it is a label then check its legality and skip it*/
	if(word[strlen(word)-1]==':'){
		word[strlen(word)-1]='\0';/*remove the ':'*/
		if(!(labelLegality(word))){
			errorHandle(SC,'i');
			return;
		}
		line=(strchr(line,':')+1);/*remove the label*/
		if(line[0]!=' '&& line[0]!='\t'){/*if there was no space between label and next word*/
			errorHandle(SC,'s');
			return;
		}
	}

	/*reads the word after entry while checking for spaces and extra words after label name*/
	if (sscanf(line," .entry%*[ 	] %s %s ",label,extra)!=1){
		errorHandle(SC,'e');
		return;
	}
	if((l=searchTable(label))==NULL){/*if label isnt in the table*/
		errorHandle(SC,'u');
		return;
	}
	if(l->attribute[0]=='e'){/*if label already extern*/
		errorHandle(SC,'b');/*label cannot be both entry and external*/
		return;
	}
	else
		l->attribute[1]='a';
	entFlag(TRUE);/*turn on entry flag*/
	return;
}
