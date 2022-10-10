/*
 * first-pass.c Does the first pass according to mmn 14 algorithm
 *
 *  Created on: Mar 7, 2021
 *      Author: Mark Gurevich
 */
#include "header.h"
#include "compilationHeader.h"
#include "Tables.h"

void addLabel(int,char*,char*);
void parse(char*);
void addData(int,char*);
void addCode(char*,char*,char*,int);

/*This function reads a line at a time and compiles it according to first pass algorithm*/
void firstPass(FILE *fp){
	char line[LINE];
	char c;
	while(!feof(fp)){
		if(!(fgets(line,81,(FILE*)fp))){/*if returned null or we reached EOF*/
			if(feof(fp)){/*if EOF*/
				return;
			}
			else{/*error occured*/
				printf("Error: Failed to read line %d",SC);
				continue;
			}
		}
		SC++;/*after reading a line increase line counter*/
		if (line[strlen(line)-1]=='\n'){/*if received normal length line*/
			line[strlen(line)-1]='\0';
			parse(line);
		}

		else{/*received more char then line length allows*/
			errorHandle(SC,'l');
			while((c=getc(fp))!='\n'||c!=EOF){/*skips letters untill next line.*/
				continue;
			}
		}
	}
	rewind(fp);
	SC=0;/*reset line counter for second pass*/
	return;
}
/*this function parses the given line and continues to relevant functions according to parsing result*/
void parse (char *line){
	char *p;
	char cmd[LINE];
	char label[LINE];
	char param1[LINE];
	char param2[LINE];
	char extra[LINE];
	int param_num;
	int label_flag=0;/*used to flag lines with labels*/
	int i;

	if((p=strchr(line,';'))!=NULL){/*if theres a comment skip the line*/
		if (line[0]!=';'){/*check if line starts with the ';' comment sign*/
			errorHandle(SC,';');
			return;
		}
		return;
	}
	/*if this is a labeled line:*/
	if ((p=strchr(line,':'))!=NULL){
		label_flag=TRUE; /*turns label flag on*/
		sscanf(line," %s",label); /*read the first word which should be the label*/

		if (label[strlen(label)-1]!=':'){ /*if the last letter of the first word isn't ':' then its not a legal label*/
			errorHandle(SC,':');
			return;
		}
		line=p+1; /*skips the label */
		if(line[0]!=' ' && line[0]!='\t' &&line[0]!='\0'){/*if there wasnt space after label*/
			errorHandle(SC,'s');
			return;
		}
	}
	/*checks if this is a directive data line*/
	if ((p=strchr(line,'.'))!=NULL){
		/*checks if this line is a data declaration*/
		if(!(strncmp(p,".string",7)*strncmp(p,".data",5))){
			if(label_flag==TRUE){ /*adds label if needed*/
				addLabel(DC,label,(strncmp(p,".string",7)==0? "s":"d"));
			}
			addData(DC,line);
			return;
		}
		/*checks if this .entry or .extern*/
		if(!(strncmp(p,".extern",7)*strncmp(p,".entry",6))){
			if(!(strncmp(p,".entry",6))) /*if its entry it will be handled in second pass*/
				return;
			sscanf(line," %s",extra);/*read the first word*/
			if(strcmp(extra,".extern")!=0){/*if its not extern its not legal*/
				errorHandle(SC,'s');
				return;
			}
			line = strstr(line, ".extern")+7;/*remove the word .extern from the line*/
			i=0;
			while(line[i]==' '||line[i]=='\t'){/*skip spaces*/
				i++;
			}
			if(i==0){/*if there were no spaces after extern its an error*/
				errorHandle(SC,'s');
				return;
			}
			line =line+i;/*skips the spaces*/
			if(sscanf(line," %s %s ",label,extra)!=1){/*read the label and if there was no label after .extern its an error*/
				errorHandle(SC,'s');
				return;
			}
			addLabel(0,label,"e");
			return;
		}
		else{/*unrecognized directive attempt*/
			errorHandle(SC,'a');
			return;
		}
	}
	/*Check if its a code line*/
	else if((param_num=(sscanf(line," %s %[^, ] , %[^, ] %s",cmd,param1,param2,extra))-1)>2){
		errorHandle(SC,'e');/*error if command has 3 parameters*/
		return;
	}
	if (label_flag==TRUE){/*add label if needed*/
		if (param_num>=0){
			addLabel(IC,label,"c");
		}
		else
			addLabel(IC,label,"");
	}
	if(param_num>=0){/*if the line wasnt blank*/
		addCode(cmd,param1,param2,param_num);
	}
	return;
}
/*this function checks which operand type is passed parameter*/
int paramType(char* param){
	int type=-1;
	int i;
	char *regs[8]={"r0","r1","r2","r3","r4","r5","r6","r7"};

	if (param[0]=='#')/*if param is type 0(immediate)*/
		type=0;

	else if(param[0]=='%'){/*if param is type 2(relative)*/
		type=2;
	}

	else {
		for(i=0;i<REGS;i++){/*checks if param is type 3(register)*/
			if((strcmp(param,regs[i]))==0){
				type=3;
				break;
			}
		}
	}
	if (type==-1)/*if its not type 0,2,3, then assume it is 1*/
		type=1;

	return type;
}
/*this function adds an operands coding to the code table*/
void addParam(char *param,int type){
	codeobj *c=(codeobj*)malloc(sizeof(codeobj));
	int i=0;
	char *regs[8]={"r0","r1","r2","r3","r4","r5","r6","r7"};

	c->address=IC;
	if(type==0){/*if param is of type direct*/
		param++;/*skip the '#'*/
		if(strlen(param)==0){/*invalid type 0 operand*/
			errorHandle(SC,'w');
			free(c);
			return;
		}
		if(param[0]=='+')
			param++;/*skip the '+'*/
		if (param[0]=='-')
			i++;
		/*check if legal number*/
		while(i<strlen(param)){
			if(!(isdigit(param[i]))){
				errorHandle(SC,'w');
				free(c);
				return;
			}
			i++;
		}
		c->code=atoi(param);/*read the number*/
		if(c->code>MAXINT||c->code<MININT){/*check for overflow*/
			errorHandle(SC,'o');
			free(c);
			return;
		}
		c->dependency='A'; /*dependancy is A because its type 1 param and thus no label is used*/
		c->labelused=NULL;
		c->labelLine=-1;
	}
	else if(type==1){
		if(labelLegality(param)==FALSE){/*check if legal label name*/
			errorHandle(SC,'i');
			free(c);
			return;
		}
		c->code=0;/*wait for second pass*/
		c->dependency='?';
		c->labelused=(char*)malloc(strlen(param)+1);
		strcpy(c->labelused,param);/*copy the name of the label for second pass*/
		c->labelLine=SC;
	}
	else if(type==2){
		if(labelLegality(param+1)==FALSE){/*check if legal label name(+1 to skip '%')*/
			errorHandle(SC,'i');
			free(c);
			return;
		}
		c->code=0;/*wait for second pass*/
		c->dependency='?';
		c->labelused=(char*)malloc(strlen(param)+1);
		strcpy(c->labelused,param);
		c->labelLine=SC;
	}
	else if(type==3){
		for(i=0;i<REGS;i++){
			if(strcmp(param,regs[i])==0){
				break;
			}
		}
		c->code=1<<i;
		c->dependency='A';
		c->labelused=NULL;
		c->labelLine=-1;
	}
	c->next=NULL;
	clast->next=c;
	clast=c;
	IC++;
	return;
}
/*this function parses and adds code lines to the code table*/
void addCode(char* cmd,char* param1,char* param2,int param_num){
	int commandNum;
	int instruction;
	int src_type=-1;/*-1 means that this instruction doesnt use this parameter*/
	int des_type=-1;
	int flag=0;
	codeobj *command;
	struct command command_list[COMMANDS]={ /*Initialize elements with:
	[name],[opcode],[funct],[number of parameters],[valid source parameter types],
	[destination parameter type],[parameter types] where:0={no param},1={1},2={1,2},3={1,3},4={0,1,3}*/
			{"mov",0,0,2,4,3},
			{"cmp",1,0,2,4,4},
			{"add",2,10,2,4,3},
			{"sub",2,11,2,4,3},
			{"lea",4,0,2,1,3},
			{"clr",5,10,1,0,3},
			{"not",5,11,1,0,3},
			{"inc",5,12,1,0,3},
			{"dec",5,13,1,0,3},
			{"jmp",9,10,1,0,2},
			{"bne",9,11,1,0,2},
			{"jsr",9,12,1,0,2},
			{"red",12,0,1,0,3},
			{"prn",13,0,1,0,4},
			{"rts",14,0,0,0,0},
			{"stop",15,0,0,0,0}};

	for(commandNum=0;commandNum<COMMANDS;commandNum++){/*find the command*/
		if((strcmp(cmd,command_list[commandNum].name))==0)
			break;
	}
	if(commandNum==COMMANDS){/*if command not found*/
		errorHandle(SC,'c');
		return;
	}
	if (command_list[commandNum].param_num!=param_num){/*if wrong number of parameters for command*/
		errorHandle(SC,'n');
		return;
	}
	instruction = command_list[commandNum].op<<8; /*load op field in code word*/
	instruction |= command_list[commandNum].funct<<4; /*load funct field in code word*/

	switch (param_num){
		case 0:/*command takes no operands*/
			break;
		case 1:/*command takes only destination operand*/
			des_type=paramType(param1);
			instruction |= des_type;
			break;

		case 2:/*command takes both destination and source operand*/
			src_type=paramType(param1);
			des_type=paramType(param2);
			instruction |= src_type<<2;
			instruction |= des_type;
			break;
	}
	switch (src_type){/*checks if passed operands are legal for this command*/
		case -1:/*means this function doesnt have src parameter*/
			if(command_list[commandNum].src_type!=0)
				flag=1;
			break;
		case 1:
			if(command_list[commandNum].src_type!=1 && command_list[commandNum].src_type!=4)
				flag=1;
			break;
		case 0:
		case 3:
			if(command_list[commandNum].src_type!=4)
				flag=1;
			break;
		case 2:
			flag=1;
			break;

	}
	switch (des_type){
		case -1:/*means this function doesnt have des parameter*/
			if(command_list[commandNum].des_type!=0)
				flag=1;
			break;
		case 1:
			if(command_list[commandNum].des_type<2)
				flag=1;
			break;
		case 0:
			if(command_list[commandNum].des_type!=4)
				flag=1;
			break;
		case 3:
			if(command_list[commandNum].des_type<3)
				flag=1;
			break;
		case 2:
			if(command_list[commandNum].des_type!=2)
				flag=1;
			break;
	}
	if (flag==TRUE){
		errorHandle(SC,'p');
		return;
	}
	/*add the instruction to code table*/
	command=(codeobj*)malloc(sizeof(codeobj));
	command->address=IC;
	command->code=instruction;
	command->dependency='A';
	command->labelused=NULL;
	command->labelLine=-1;
	command->next=NULL;
	if(IC==100){
		IC++;
		cfirst=command;
		clast=command;
	}
	else{
		IC++;
		clast->next=command;
		clast=command;
	}

	switch(param_num){
		case 0:
			break;
		case 1:
			addParam(param1,des_type);
			break;
		case 2:
			addParam(param1,src_type);
			addParam(param2,des_type);
			break;
	}
	return;
}
/*this function adds a passed number into the data table*/
void addnum(char* num){
	int value;
	int i=0;
	dataobj *d;

	if(num[0]=='+')
		num++;
	if(num[0]=='-'){
		i++;
	}
	while(i<strlen(num)){/*check if legal number was entered*/
		if(!(isdigit(num[i]))){
			errorHandle(SC,'w');
			return;
		}
		i++;
	}
	value=atoi(num);
	if(value>MAXINT||value<MININT){/*checks if number is in bounds*/
		errorHandle(SC,'o');
		return;
	}
	/*Initialize the new data object*/
	d=(dataobj*)malloc(sizeof(dataobj));
	d->code=value;
	d->next=NULL;
	if (DC==0){
		dfirst=d;
		dlast=d;
	}
	else{
		dlast->next=d;
		dlast=d;
	}
	DC++;/*increment counter*/
}
/*this function adds a passed string into the data table*/
void addString(char* string){
	int i=0;
	dataobj *d;

	while(string[i]!='\0'){
		d=(dataobj *)malloc(sizeof(dataobj));/*for each characters creates a dataobj*/
		d->code=(int)string[i];
		d->next=NULL;
		if (DC==0){
			dfirst=d;
			dlast=d;
		}
		else{
			dlast->next=d;
			dlast=d;
		}
		DC++;/*increment counter*/
		i++;
	}
	d=malloc(sizeof(dataobj));/*adds the '\0' at the end of string*/
	d->code=0;/*code for '\0'*/
	d->next=NULL;
	if (DC==0){
		dfirst=d;
		dlast=d;
	}
	else{
		dlast->next=d;
		dlast=d;
	}
	DC++;/*increment counter*/
	return;
}
/*this function parses and adds .data or .string Data types.*/
void addData(int address, char *line){
	char *p=(char*)malloc(LINE);
	char current[80];
	int i=0,j=0;
	int flag=1; /*this flags checks if ',' is valid as next input for .data or  '\"' for .string */

	sscanf(line ," %s",p);/*read the next word to determine its data type*/

	if(strcmp(p,".data")==0){/*if its numeric data*/
		for(i=0;isspace(line[i]);i++);/*skip spaces*/
		line=line+i+5;/*removes the word .data*/
		if (line[0]!=' '&&line[0]!='\t'){/*if theres no space or tab following*/
			errorHandle(SC,'s');
			return;
		}
		i=0;
		while(line[i]!='\0'){/*parse the line*/
			if (line[i]==' '||line[i]=='\t'){/*ignores all  spaces and tabs*/
				i++;
				continue;
			}
			if(line[i]==','){ /*checks if current char is ','*/
				if (flag==1){/*this mean we have an illegal ','*/
					errorHandle(SC,'s');
					return;
				}
				else{/*if legal ',' it means we finished reading a number*/
					flag=1;
					current[j]='\0';
					addnum(current);/*add the number we read to the data table*/
					j=0;
					i++;
					continue;
				}
			}
			if(isdigit(line[i])!=0||line[i]=='+'||line[i]=='-'){/*checks if current char is legal to add*/
				if(j!=0 &&(isspace(line[i-1]))){/*this prevents spaces inside numbers*/
					errorHandle(SC,'w');
					return;
				}
				flag=0; /*this mean our next input can be ','*/
				if((line[i]=='+'||line[i]=='-')&&j>0){/*if the '-' or '+' not at the start of the number*/
					errorHandle(SC,'w');
					return;
				}
				current[j]=line[i];
				j++;
				i++;
			}
			else{/*in this case its an illegal input for .data instruction*/
				errorHandle(SC,'w');
				return;
			}
		}
		if(flag==1){/*meaning we ended the line on ','*/
			errorHandle(SC,'s');
			return;
		}
		/*adds the last number on the list*/
		current[j]='\0';
		addnum(current);
		free(p);
		return;
	}

	else if(strcmp(p,".string")==0){/*if its a .string type data*/
		for(i=0;isspace(line[i]);i++);
		line=line+i+7;/*removes the ".string" from line*/
		if (line[0]!=' ' && line[0]!='\t'){/*if theres no whitespace after deceleration .string*/
			errorHandle(SC,'s');
			return;
		}
		i=0;
		/*parse the string*/
		while(line[i]!='\0'){
			if(line[i]=='\"'&& flag ==1){/*if its the first \" we enter the string */
					flag=0;
					i++;
					continue;
			}
			if(flag!=0){/*if we're outside the string*/
				if(line[i]!=' '&&line[i]!='\t'){/*if we encounter a char outside of string its an error*/
					errorHandle(SC,'s');
					return;
				}
				else{/*else its a space character so skip it*/
					i++;
					continue;
				}
			}
			if(line[i]=='\"'&& flag==0){/*second \" which closes the string*/
				flag=-1;
				i++;
				continue;
			}

			/*if reached here then its a character inside the string*/
			if(isprint(line[i])==FALSE){/*check if its a legal character*/
				errorHandle(SC,'h');
				return;
			}
			current[j]=line[i];
			i++;
			j++;
		}
		if(flag!=-1){
			errorHandle(SC,'s');
			return;
		}
		current[j]='\0';
		addString(current);
	}

	else{/*else the syntax of the data deceleration wasnt valid */
		errorHandle(SC,'d');
		return;
	}
}
/*this function adds a label to the symbol table*/
void addLabel(int address,char *label,char *type){
	labelobj *l;

	if(label[strlen(label)-1]==':'){/*removes the ':' from the end of the label*/
		label[strlen(label)-1]='\0';
	}
	/*checks if label is legal:*/
	if(!(labelLegality(label))){
		errorHandle(SC,'i');
		return;
	}
	/*checks if label is already in the table*/
	if((l=searchTable(label))!=NULL){
		if(l->attribute[0]=='e'&&type[0]=='e'){/*legal to redeclare labels as extern*/
			return;
		}
		errorHandle(SC,'m');
		return;
	}
	/*if reached here then label is legal and can be created and inserted in symbol table*/
	l=(labelobj*)malloc(sizeof(labelobj));
	l->name=(char*)malloc(strlen(label)+1);
	strcpy(l->name,label);
	l->address=address;
	l->next=NULL;
	l->attribute[0]=type[0];
	l->attribute[1]='\0';
	if (type[0]=='e')
		extFlag(TRUE);

	if(LC==0){
		lfirst=l;
		llast=l;
	}
	else{
		llast->next=l;
		llast=l;
	}
	LC++; /*increment label counter*/
}
