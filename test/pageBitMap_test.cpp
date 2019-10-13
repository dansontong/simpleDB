#include <stdio.h>
#include "config.h"

int main()
{
	unsigned long *bit_map = (unsigned long *)malloc(BIT_MAP_SIZE);
	int position = 1;
	unsigned long result = 0x0000000000000001;

	printf("sizeof(unsigned long):%ld\n", sizeof(result));//Intel-Core-i7-8700-CPU中long类型长度是8个字节，也就是64位。
	printf("sizeof(long):%ld\n", sizeof(result));//Intel-Core-i7-8700-CPU中long类型长度是8个字节，也就是64位。

	result = result<<(64-position);
	result = result & *bit_map;
	if (result == 0) {
		return 0;
	}
	else {
		return 1;
	}
}