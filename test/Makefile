cc = g++

CXXFLAGS = -pthread -g -Ddebug
INCLUDE = -I ../storage
SRC = -L../storage/*.cpp

buffer: buffer_test.cpp
	$(cc) $(CXXFLAGS) $(INCLUDE) buffer_test.cpp ../storage/buffer.cpp -o buffer_test
	./buffer_test
	rm -rf buffer_test *.log
