#不生成.o文件，一步到位得到可执行文件
cc = g++
target = storage
INCLUDE = -I ./log
# debug mode default
ver = debug
ifeq ($(ver), debug)
# ALL: storage_d
target = storage_d
CXXFLAGS = -pthread -g -Ddebug
else
# ALL: storage_r
target = storage_r
CXXFLAGS = -pthread -O3
endif

head = $(shell find ./log ./storage -name "*.h")
src = $(shell find ./log ./storage -name "*.cpp")

$(target): $(src)
	$(cc) $(CXXFLAGS) $(INCLUDE) $^ -o $(target)
	# ./$(target)
	# rm $(target)

clear:
	rm $(target)