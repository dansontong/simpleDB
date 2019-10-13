#include "page.h"

int main()
{
	unsigned long *bit_map = (unsigned long *)malloc(DB->dbMeta.bitMapSize);
	int position = 1;
	unsigned long result = 0x0000000000000001;

	printf("%d\n", sizeof(result));
	
	result = result<<(64-position);
	result = result & bit_map;
	if (result == 0) {
		return 0;
	}
	else {
		return 1;
	}
}