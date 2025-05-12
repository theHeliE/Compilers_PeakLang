BISON = "C:/MinGW/GnuWin32/bin/bison"
FLEX = "C:/MinGW/GnuWin32/bin/flex"

all: compiler.exe

compiler.exe:
	$(BISON) -d -v parser.y
	$(FLEX) lexer.l
	g++ -c SymbolTable.cpp -o SymbolTable.o
	g++ -c parser.tab.c -o parser.tab.o 
	g++ -c lex.yy.c -o lex.yy.o
	g++ SymbolTable.o lex.yy.o parser.tab.o -o compiler.exe -std=c++11 -static-libgcc -static-libstdc++

test1: compiler.exe
	./compiler.exe ./tests/test1.txt

test-all: test1

counterexample:
	$(BISON) -d -v parser.y -Wcounterexamples

clean:
	rm compiler.exe parser.tab.c parser.tab.h lex.yy.c
