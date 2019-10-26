#不生成.o文件，一步到位得到可执行文件
cc = g++
target = database
INCLUDE = -I ./log -I ./core  -I ./storage
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
src = $(shell find ./log ./core ./storage -name "*.cpp")

$(target): $(src)
	$(cc) $(CXXFLAGS) $(INCLUDE) $^ main.cpp -o $(target)
	# ./$(target)
	# rm $(target)

clear:
	rm $(target)