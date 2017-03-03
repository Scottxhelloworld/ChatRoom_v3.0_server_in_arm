#include "include.h"

char * get_current_time()
{
	sqlite3 * db;
	char ** result;
	char * errmsg;
	char sql[64];

	int nrows;
	int ncols;
	int ret;

	if((ret = sqlite3_open("./database.db",&db)) != SQLITE_OK)
	{
		printf("error \n");
		return NULL;
	}

	sprintf(sql,"select datetime('now','localtime');");
	
	if((ret = sqlite3_get_table(db,sql,&result,&nrows,&ncols,&errmsg)) != SQLITE_OK)
	{
		printf("get current time error\n");
		return NULL;
	}
	else
	{
		return result[1];
	}
}
