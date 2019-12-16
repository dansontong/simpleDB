#不生成.o文件，一步到位得到可执行文件
cc = g++
target = database
INCLUDE = -I ./log -I ./core -I ./index -I ./index/BPlusTree -I ./storage -I ./operator -I ./executor -I ./lib -I ./sqlparser
# debug mode default
ver = debug
ifeq ($(ver), debug)
# ALL: database_d
target = database_d
CXXFLAGS = -pthread -g -Ddebug -fpermissive
else
# ALL: database_r $(head)
target = database_r
CXXFLAGS = -pthread -O3 -fpermissive
endif

head = $(shell find ./log ./core ./storage -name "*.h")
src = $(shell find ./log ./core ./storage ./operator ./index ./executor ./sqlparser ./lib -name "*.cpp") 

$(target): $(src)
	$(cc) $(CXXFLAGS) $(INCLUDE) ./main.cpp $(src) -o $(target)
	# ./$(target)
	# rm $(target)

clear:
	rm $(target)
