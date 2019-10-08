cc = g++
target = storage

head = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.cpp")

obj = $(src:%.c=%.o)

$(target):$(obj)
	$(cc) -o $(target) $(obj)

$.o:%.cpp $(head)
	$(cc) -c $< -o $@

clean:
	rm -rf $(obj) $(target)

#main:main.cpp
#	g++ -o main main.cpp 
