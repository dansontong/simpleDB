#
# makefile for sqlparser
#
OBJS += -L../log/log.cpp lex.cpp parse.cpp parse.h trivialtree.h trivialtree.cpp sqlparser.h sqlparser.cpp 

sqlparser: sqlparser.h sqlparser.cpp parse.cpp parse.h lex.cpp
	g++ -pthread -g -w -fpermissive -I../log $(OBJS) -osqlparser 

# dependencies
depends:
	flex -olex.cpp -i lex.l
	bison -o parse.c parse.y -d
	mv parse.c parse.cpp
	
clean:
	rm -rf *.o
	rm -rf sqlparser

# source
parse.cpp: parse.y

lex.cpp: lex.l
