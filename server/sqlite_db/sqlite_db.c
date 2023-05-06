#include "sqlite_db.h"

sqlite3 *create_db(void){
	sqlite3 *ppdb;
	char *errmsg;
	int ret = sqlite3_open("qq.db",&ppdb);
	if(ret != SQLITE_OK){
		perror("数据库打开失败\n");
		exit(-1);
	}
	printf("数据库打开成功\n");
	return ppdb;
}
