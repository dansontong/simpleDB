#自动编译所有文件
# cc = g++
# target = storage

# head = $(shell find ./ -name "*.h")
# src = $(shell find ./ -name "*.cpp")

# obj = $(src:%.c=%.o)

# $(target):$(obj)
# 	$(cc) -o $(target) $(obj)

# $.o:%.cpp $(head)
# 	$(cc) -c $< -o $@

# clean:
# 	rm -rf $(obj) $(target)

#main:main.cpp
#	g++ -o main main.cpp 



#增加debug功能
# cc = g++
# target = storage

# ver = debug
# ifeq ($(ver), debug)
# ALL: storage_d
# target = storage_d
# CXXFLAGS = -c -g -Ddebug
# else
# ALL: storage_r
# target = storage_r
# CXXFLAGS = -c -O3
# endif

# head = $(shell find ./ -name "*.h")
# src = $(shell find ./ -name "*.cpp")

# obj = $(src:%.cpp=%.o)

# $(target):$(obj)
# 	$(cc) -o $(target) $(obj)

# $.ro:%.cpp
# 	$(cc) $(CXXFLAGS) $< -o $@

# $.do:%.cpp
# 	$(cc) $(CXXFLAGS) $< -o $@

# clean:
# 	rm -rf $(obj) $(target)




#不生成.o文件，一步到位得到可执行文件
cc = g++
target = storage

ver = debug
ifeq ($(ver), debug)
ALL: storage_d
target = storage_d
CXXFLAGS = -pthread -g -Ddebug
else
ALL: storage_r
target = storage_r
CXXFLAGS = -pthread -O3
endif

head = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.cpp")

$(target):$(src)
	$(cc) $(CXXFLAGS) $^ -o $(target)
