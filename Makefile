#不生成.o文件，一步到位得到可执行文件
cc = g++
target = database
INCLUDE = -I ./log -I ./core -I ./index -I ./index/BPlusTree -I ./storage -I ./operator
# debug mode default
ver = debug
ifeq ($(ver), debug)
# ALL: database_d
target = database_d
CXXFLAGS = -pthread -g -Ddebug
else
# ALL: database_r
target = database_r
CXXFLAGS = -pthread -O3
endif

head = $(shell find ./log ./core ./storage -name "*.h")
src = $(shell find ./log ./core ./storage ./operator ./index -name "*.cpp")  ./main.cpp

$(target): $(src) $(head)
	$(cc) $(CXXFLAGS) $(INCLUDE) $(src) -o $(target)
	# ./$(target)
	# rm $(target)

clear:
	rm $(target)