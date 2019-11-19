all:
	flex sqllexical.lex
	mv lex.yy.c sqllexical.c
	bison sqlparser.y -o sqlparser.c -d
	g++ -g -w sqlparser.c sqlparser.h trivialtree.h trivialtree.cpp -o parser_test
	
clean:
	rm -rf sqllexical.c sqlparser.h sqlparser.c parser_test
