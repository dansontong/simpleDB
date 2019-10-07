
CFLAGS = -std=c++1z -lstdc++ -Wall -Werror -I./parser/ -L./
export LD_LIBRARY_PATH=/home/danson/文档/database/db1-storage

all:
	$(CXX) $(CFLAGS) main.cpp -o main -lsqlparser

# run follow command, before run ./example
# export LD_LIBRARY_PATH=~/文档/database/sql-parser-master

