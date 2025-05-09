all:
		win_bison -d -v parser.y
		win_flex lexer.l
		gcc parser.tab.c lex.yy.c -o compiler.exe
