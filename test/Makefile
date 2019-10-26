cc = g++

CXXFLAGS = -pthread -g -Ddebug
INCLUDE = -I ../storage -I ../log
CFLAGS_parser = -std=c++1z -lstdc++ -Wall -Werror -I../parser -L../parser -lsqlparser
SRC = -L../storage/*.cpp

buffer: storage/buffer_test.cpp
	# $(cc) $(CXXFLAGS) $(INCLUDE) storage/buffer_test.cpp ../storage/buffer.cpp ../log/log.cpp -o buffer_test.o
	$(cc) $(CXXFLAGS) $(INCLUDE) log/log_test.cpp ../log/log.cpp -o log_test.o
	$(cc) $(CXXFLAGS) $(INCLUDE) storage/pageBitMap_test.cpp  -o pageBitMap_test.o
	$(CXX) $(CFLAGS_parser) parser/parser_test.cpp -o parser_test.o -lsqlparser
	# $(cc) $(CXXFLAGS) $(INCLUDE) index/index_test.cpp  -o index_test.o
	$(cc) $(CXXFLAGS) $(INCLUDE) index/bPlusTree_test.cpp  -o bPlusTree_test.o
	# ./buffer_test.o
	./log_test.o
	./pageBitMap_test.o
	./parser_test.o
	# ./index_test.o
	 ./bPlusTree_test.o
	rm -rf *_test.o *.log

# make clear: clear runnable file 
clear:
	rm -rf *test.o  *.log

# run follow command, before run ./parser_test
# export LD_LIBRARY_PATH=..
