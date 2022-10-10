Assembler_exe: main.o  File-handeler.o first-pass.o second-pass.o compailer.o errors.o Tables.o
	gcc -g -Wall -ansi -pedantic main.o  File-handeler.o first-pass.o second-pass.o compailer.o errors.o Tables.o -o Assembler_exe

main.o: main.c
	gcc -g -Wall -ansi -pedantic main.c -c
	
File-handeler.o: File-handeler.c
	gcc -g -Wall -ansi -pedantic File-handeler.c -c

compailer.o: compailer.c
	gcc -g -Wall -ansi -pedantic compailer.c -c	

errors.o: errors.c
	gcc -g -Wall -ansi -pedantic errors.c -c	

first-pass.o: first-pass.c
	gcc -g -Wall -ansi -pedantic first-pass.c -c

second-pass.o: second-pass.c
	gcc -g -Wall -ansi -pedantic second-pass.c -c	

Tables.o: Tables.c
	gcc -g -Wall -ansi -pedantic Tables.c -c	
	
	
clear:
	rm *.o
