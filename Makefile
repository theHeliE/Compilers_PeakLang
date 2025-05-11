BISON = "C:/Program Files (x86)/GnuWin32/bin/bison"
FLEX = "C:/Program Files (x86)/GnuWin32/bin/flex"

all: compiler.exe

compiler.exe:
	$(BISON) -d -v parser.y
	$(FLEX) lexer.l
	gcc parser.tab.c lex.yy.c -o compiler.exe

test1: compiler.exe
	./compiler.exe < tests/test1.txt

test-all: test1

clean:
	rm compiler.exe parser.tab.c parser.tab.h lex.yy.c
