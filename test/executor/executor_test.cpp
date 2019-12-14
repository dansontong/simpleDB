
#include <stdlib.h>

#include <string.h>

#include "trivialtree.h"
#include "executor.h"
#include "log.h"

int main() {
    log_init("./testlog.txt");
    printf("\n======================= executor_test begin =======================\n");

    char sql[] = "select * from a,b where a.a1 = b.a1;";
    printf("sql: %s\n", sql);

    execute_sql(sql, strlen(sql));
    
    printf("\n======================= executor_test end =======================\n");
}
