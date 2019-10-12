#include "log.h"

int main(int argc, char* argv[]) {
	log_init();
	printf("hhhhhh\n");
	log_Info("logging.h test success!, this is a Info.");
}

// 使用说明：
// g++ -pthread test_log.cpp log.h -o test_log
// ./test_log