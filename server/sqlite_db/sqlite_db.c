#include "sqlite_db.h"

//打开数据库
sqlite3 *open_db(void){
	sqlite3 *ppdb;
	char *errmsg;
	int ret = sqlite3_open("qq.db",&ppdb);
	if(ret != SQLITE_OK){
		perror("数据库打开失败");
		exit(-1);
	}
	printf("数据库打开成功");
	return ppdb;
}
//关闭数据库
void close(sqlite3 *ppdb){
	int ret = int sqlite3_close(ppdb);
	if(ret != SQLITE_API){
		perror("数据库关闭失败");
		exit(-1);
	}
	printf("数据库关闭成功\n");
}
//添加数据
void exec_add(char *sql,sqlite3 *ppdb){

}
//删除数据
void exec_del(char *sql,sqlite3 *ppdb){

}
//修改数据
void exec_update(char *sql,sqlite3 *ppdb){

}
//查询数据
void exec_select(char *sql,sqlite3 *ppdb){

}
