assembler: assembler.o first_pass.o second_pass.o data_functions.o functions_first_pass.o instruction_functions.o macro_deployment.o
	gcc -ansi -Wall -pedantic -g assembler.o first_pass.o second_pass.o data_functions.o functions_first_pass.o instruction_functions.o macro_deployment.o -o assembler

assembler.o: assembler.c defines.h functions.h globals.h structs.h
	gcc -ansi -Wall -pedantic -g -c assembler.c

first_pass.o: first_pass.c defines.h functions.h globals.h structs.h
	gcc -ansi -Wall -pedantic -g -c first_pass.c

second_pass.o: second_pass.c defines.h functions.h globals.h structs.h
	gcc -ansi -Wall -pedantic -g -c second_pass.c

data_functions.o: data_functions.c defines.h functions.h globals.h structs.h
	gcc -ansi -Wall -pedantic -g -c data_functions.c

functions_first_pass.o: functions_first_pass.c defines.h functions.h globals.h structs.h
	gcc -ansi -Wall -pedantic -g -c functions_first_pass.c

instruction_functions.o: instruction_functions.c defines.h functions.h globals.h structs.h
	gcc -ansi -Wall -pedantic -g -c instruction_functions.c

macro_deployment.o: macro_deployment.c defines.h functions.h globals.h structs.h
	gcc -ansi -Wall -pedantic -g -c macro_deployment.c
