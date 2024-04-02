
scanner.o : scanner.c scanner.h
	gcc -Wall -g -c scanner.c

symbolTable.o : symbolTable.c symbolTable.h
	gcc -Wall -g -c symbolTable.c

parser.o : parser.c
	gcc -Wall -g -c parser.c

driver.o : driver.c 
	gcc -Wall -g -c driver.c

ast.o : ast.c ast.h
	gcc -Wall -g -c ast.c

ast-print.o : ast-print.c
	gcc -Wall -g -c ast-print.c

generateCode.o : generateCode.c generateCode.h
	gcc -Wall -g -c generateCode.c

postOrder.o : postOrder.c postOrder.h
	gcc -Wall -g -c postOrder.c

compile : scanner.o parser.o driver.o symbolTable.o ast.o postOrder.o ast-print.o generateCode.o
	gcc -Wall -g scanner.o symbolTable.o parser.o driver.o ast.o ast-print.o postOrder.o generateCode.o -o compile

clean :
	rm -f scanner compile *.o