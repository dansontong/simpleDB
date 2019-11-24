#
# makefile for sqlparser
#
OBJS += lex.c parse.c parse.h trivialtree.h trivialtree.cpp sqlparser.h sqlparser.cpp

sqlparser: sqlparser.h sqlparser.cpp parse.c parse.h lex.c
	g++ -g -w -fpermissive -osqlparser $(OBJS)

# dependencies
depends:
	flex -olex.c -i lex.l
	bison -o parse.c parse.y -d

clean:
	rm -rf *.o
	rm -rf sqlparser

# source
parse.c: parse.y

lex.c: lex.l
