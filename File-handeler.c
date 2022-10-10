/*
 * File-handeler.c handles opening and creating files
 *
 *  Created on: Jan 28, 2021
 *      Author: Mark Gurevich
 */

#include "header.h"
#include "Tables.h"
#include "compilationHeader.h"

void buildFile(char *);

/*this function opens a file and sends it for compilation, if successful continues to create files.*/
void readFile(char *filename){
	FILE *fp;

	fp= fopen(filename,"r");
	if (!fp){
		printf("Error: file: %s, not recognized\n",filename);
		return;
	}
	printf("-----Now compiling file: \"%s\"-----\n",filename);

	compile(fp);/*if file located then send it for compilation*/
	fclose(fp);/*close the file*/
	if((errorStatus(0))==TRUE){/*checks if there were errors with compilation.*/
		printf("Assembly for file \"%s\" has failed.\n",filename);
		reset();/*reset tables*/
		errorStatus(-1);/*resets error status.*/
		return;
	}
	/*if compilation was successful:*/
	buildFile(filename);
	reset();
	errorStatus(-1);/*resets error status.*/


	printf("Assembly for file \"%s\" succeeded.\n",filename);

	return;
}

void buildFile(char* filename){
	extern int IC,DC;
	int i;
	int length=strlen(filename);
	char *name;
	char *objname=(char*)malloc(length);
	char *extname=(char*)malloc(length);
	char *entname=(char*)malloc(length);
	FILE *obj;
	FILE *ext;
	FILE *ent;
	labelobj *l=lfirst;
	dataobj *d=dfirst;
	codeobj *c=cfirst;


	name=(char*)malloc(length);
	strncpy(name,filename,(length-3));/*removes the .as from the filename*/
	name[length-3]='\0';

	/*create the .obj file*/
	strcpy(objname,name);
	strcat(objname,".obj");
	if((obj=fopen(objname,"w"))==NULL){
		printf("Error creating .obj file.");
	}
	fprintf(obj,"  %d %d\n",IC-100,DC);/*print the title*/
	while(c){
		fprintf(obj,"%d %.3X %c\n",c->address ,c->code&BITS ,c->dependency);
		c=c->next;
	}
	i=IC;
	while(d){
		fprintf(obj,"%d %.3X %c\n",i,d->code&BITS,'A');
		i++;
		d=d->next;
	}

	/*Creates the ext file if needed:*/
	if((extFlag(0))==TRUE){
		strcpy(extname,name);
		strcat(extname,".ext");
		if((ext=fopen(extname,"w"))==NULL){
			printf("Error creating .ext file");
		}
		c=cfirst;
		while((c)){
			if(c->dependency == 'E'){/*print only the external dependent lines*/
				fprintf(ext,"%s %d\n",c->labelused,c->address);
			}
			c=c->next;
		}
	}
	/*Creates the .ent file if needed:*/
	if((entFlag(0))==TRUE){
		strcpy(entname,name);
		strcat(entname,".ent");
		if((ent=fopen(entname,"w"))==NULL){
			printf("Error creating .ent file");
		}
		while((l)){
			if(l->attribute[1]=='a'){
				fprintf(ent,"%s %d\n",l->name,l->address);
			}
			l=l->next;
		}
	}
	free(entname);
	free(extname);
	free(objname);
	free(name);
	return;
}
