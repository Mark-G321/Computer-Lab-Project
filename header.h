/*
 * header.h is the main header for the program
 *
 *  Created on: Jan 28, 2021
 *      Author: Mark Gurevich
 */

#ifndef HEADER_H_
#define HEADER_H_
/*includes:*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
/*function prototypes:*/
void readFile(char*);
void compile(FILE *);

void errorHandle(int,char);
int errorStatus(int);


#define TRUE 1
#define FALSE 0
#define BITS 4095
#define NAME_LENGTH 100

#define ENDING ".as"

#endif /* HEADER_H_ */
