
scanner.o : scanner.c scanner.h
	gcc -Wall -g -c scanner.c

symbolTable.o : symbolTable.c symbolTable.h
	gcc -Wall -g -c symbolTable.c

parser.o : parser.c
	gcc -Wall -g -c parser.c

driver.o : driver.c 
	gcc -Wall -g -c driver.c

compile : scanner.o parser.o driver.o symbolTable.o
	gcc -Wall -g scanner.o symbolTable.o parser.o driver.o -o compile

clean :
	rm -f scanner compile *.o