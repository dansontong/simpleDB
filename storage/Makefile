#不生成.o文件，一步到位得到可执行文件
cc = g++
target = storage
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

head = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.cpp")

$(target): $(src)
	$(cc) $(CXXFLAGS) $^ -o $(target)
	./$(target)
	rm $(target)
