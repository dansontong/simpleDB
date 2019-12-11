#
# makefile for sqlparser
#
OBJS += lex.c parse.c parse.h trivialtree.h trivialtree.cpp sqlparser.h sqlparser.cpp ../log/log.cpp ../log/log.h

sqlparser: sqlparser.h sqlparser.cpp parse.c parse.h lex.c
	g++ -g -w -fpermissive -osqlparser $(OBJS)

# dependencies
depends:
	flex -olex.cpp -i lex.l
	bison -o parse.cpp parse.y -d

clean:
	rm -rf *.o
	rm -rf sqlparser

# source
parse.c: parse.y

lex.c: lex.l
