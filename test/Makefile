cc = g++

CXXFLAGS = -pthread -g -Ddebug
INCLUDE = -I ../storage
SRC = -L../storage/*.cpp

buffer: buffer_test.cpp
	$(cc) $(CXXFLAGS) $(INCLUDE) buffer_test.cpp ../storage/buffer.cpp ../storage/log.cpp -o buffer_test
	$(cc) $(CXXFLAGS) $(INCLUDE) log_test.cpp ../storage/log.cpp -o log_test
	./buffer_test
	./log_test
	rm -rf buffer_test log_test *.log
