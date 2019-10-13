#include "log.h"

int main(int argc, char* argv[]) {
	log_init();
	printf("hhhhhh\n");

	log_Error("logging.h test success!, this is a Info.");
	log_Warn("logging.h test success!, this is a Info.");
	log_Info("logging.h test success!, this is a Info.");
	log_Debug("logging.h test success!, this is a Info.");

	Log(FATAL, "logtemp:No.%s,char:%4f","hello", 0.1);
	Log(ERROR, "logtemp:No.%d,char:%c",5, 'd');
	Log(WARN, "logtemp:No.%s,char:%4f","hello", 0.1);
	Log(INFO, "logtemp:No.%s,char:%4f","我是插进来的字符串", 0.1);
	
	log_init();
}

// 使用说明：
// g++ -pthread test_log.cpp log.h -o test_log
// ./test_log