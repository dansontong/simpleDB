cc = g++

CXXFLAGS = -pthread -g -Ddebug
INCLUDE = -I ../storage -I ../log
SRC = -L../storage/*.cpp

buffer: buffer_test.cpp
	$(cc) $(CXXFLAGS) $(INCLUDE) buffer_test.cpp ../storage/buffer.cpp ../log/log.cpp -o buffer_test
	$(cc) $(CXXFLAGS) $(INCLUDE) log_test.cpp ../log/log.cpp -o log_test
	./buffer_test
	./log_test
	rm -rf buffer_test log_test *.log
