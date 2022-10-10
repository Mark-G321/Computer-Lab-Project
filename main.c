/*
 * main.c of assembly program.
 *
 *  Created on: Jan 28, 2021
 *      Author: Mark Gurevich
 */

#include "header.h"
int main(int argc,char *argv[]){
	int i;
	char *current=(char*)malloc(NAME_LENGTH);

	for(i=1;i<argc;i++){
		strcpy(current,argv[i]);
		strcat(current,ENDING);
		readFile(current);

	}
	free(current);
	return 0;
}

