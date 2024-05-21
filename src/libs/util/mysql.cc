#include "util.h"
#include <cstdlib>

TZone gMySQLZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CMySQL),
	0,
	0,
	NULL,   
	"Zone CMySQL"
};

CMySQL::CMySQL()
{
	mMySQL = NULL;
}

CMySQL::~CMySQL()
{
	close();
}


bool
CMySQL::open(const char *aHost, const char *aUser,
		const char *aPassword, const char *aDatabase)
{
	if (mMySQL) return false;
//	mMySQL = (MYSQL*)malloc(sizeof(MYSQL));
	mMySQL = mysql_init(NULL);

	int
		Count = 0;

	while(Count < 3)
	{
		if (mysql_real_connect(mMySQL, aHost, aUser, aPassword, aDatabase, 0, NULL, 0)) break;
		Count++;
		pth_nap((pth_time_t){2, 0});
	}

	if (Count == 3)
	{
		system_log("Failed to connect to game database: Error: %s",
		mysql_error(mMySQL));
//		free(mMySQL);
		mysql_close(mMySQL);
		mMySQL = NULL;
		return false;
	}

	return true;
}

void
CMySQL::close()
{
	if (mMySQL)
	{
		mysql_close(mMySQL);
//		free(mMySQL);
		mMySQL = NULL;
		system_log("close mysql connection");
	}
}


int
CMySQL::query(const char *aQuery)
{
	if (!mMySQL) return -1;

	int
		Res = mysql_query(mMySQL, aQuery);

	if (Res != 0)
	{
		system_log("query failed: %s", aQuery);
		system_log("error string[%d]: %s", mysql_errno(mMySQL), mysql_error(mMySQL));
	}
	return Res;
}
