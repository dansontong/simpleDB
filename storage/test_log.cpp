#include "logging.h"

int main(int argc, char* argv[]) {
	initLog();
	printf("hhhhhh\n");
	logInfo("logging.h test success!, this is a Info.");
}

// 使用说明：
// g++ -pthread main1.cpp -o test_log
// ./test_log