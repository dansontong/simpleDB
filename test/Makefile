cc = g++

CXXFLAGS = -pthread -g -Ddebug -fpermissive
INCLUDE = -I ../core -I ../storage -I ../log -I ../index/BPlusTree -I ../index -I ../executor -I ../sqlparser -I ../lib -I ../operator
CFLAGS_parser = -std=c++1z -lstdc++ -Wall -Werror -I../parser -L../parser -lsqlparser
LIB_SRC =  -L../storage/*.cpp -L../core/*.cpp
src = ../core ../storage ../index
src_buf = $(shell find ../log ../core ../storage -name "*.cpp")
src_bPtree = $(shell find ../log ../index/BPlusTree  -name "*.cpp")
src_index = $(shell find ../log ../core ../storage ../index ../sqlparser -name "*.cpp")
src_operator = $(shell find ../log ../core ../storage ../index ../operator  -name "*.cpp")
src_parser = $(shell find ../log ../sqlparser ../lib  -name "*.cpp")
src_executor = $(shell find ../log ../core ../storage ../sqlparser ../operator ../index ../lib -name "*.cpp") ../executor/executor.cpp ../executor/grammar2plan.cpp
src_optimizer = ../executor/logicOptimize.cpp

buffer: 
	# $(cc) $(CXXFLAGS) $(INCLUDE) storage/buffer_test.cpp $(src_buf) -o buffer_test.o
	# $(cc) $(CXXFLAGS) $(INCLUDE) log/log_test.cpp ../log/log.cpp -o log_test.o
	# $(cc) $(CXXFLAGS) $(INCLUDE) storage/pageBitMap_test.cpp  -o pageBitMap_test.o
	# $(CXX) $(CFLAGS_parser) parser/parser_test.cpp -o parser_test.o -lsqlparser
	# $(cc) $(CXXFLAGS) $(INCLUDE) index/index_test.cpp $(src_index) -o index_test.o
	# $(cc) $(CXXFLAGS)  -I ../index/BPlusTree index/bPlusTree_test.cpp $(src_bPtree) -o bPlusTree_test.o
	# $(cc) $(CXXFLAGS) $(INCLUDE) index/bPlusTree_test.cpp $(src_bPtree) -o bPlusTree_test.o
	# $(cc) $(CXXFLAGS) $(INCLUDE) operator/operator_test.cpp $(src_operator) -o operator_test.o
	# $(cc) $(CXXFLAGS) $(INCLUDE) parser/sqlparser_test.cpp $(src_parser) -o parser_test.o
	$(cc) $(CXXFLAGS) $(INCLUDE) executor/executor_test.cpp $(src_executor)  -o executor_test.o
	# $(cc) $(CXXFLAGS) $(INCLUDE) executor/logicOpti_test.cpp $(src_optimizer)  -o logicOpti_test.o
	# ./buffer_test.o
	# ./log_test.o
	# ./pageBitMap_test.o
	# ./parser_test.o
	# ./index_test.o
	# ./bPlusTree_test.o
	./executor_test.o
	# ./logicOpti_test.o
	# rm -rf *_test.o *.log

# make clear: clear runnable file 
clear:
	rm -rf *test.o  *.log

# run follow command, before run ./parser_test
# export LD_LIBRARY_PATH=../parser
