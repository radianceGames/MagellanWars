#include "store.h"
#include "cgi.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

TZone gStoreCenterZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CStoreCenter),
	0,
	0,
	NULL,   
	"Zone CStoreCenter"
};

CStoreCenter::CStoreCenter()
{
    lock = false;
    mRoot = NULL;
    mTail = NULL;
    mLength = 0;
}

CStoreCenter::~CStoreCenter()
{
    while (!is_empty())
    {
        delete pop();
    }    

	// TODO: remove -- called by MySQL~()
	if (mDatabaseConnection)
       mDatabaseConnection->close();
}

bool
CStoreCenter::initialize(CMySQL *aMySQL)
{
	if (!aMySQL)
	{
	   system_log("NULL MySQL Connection in CStoreCenter::initialize()"); 
       return false; 
    }    
	mDatabaseConnection = aMySQL;
	return true;
}

void CStoreCenter::push(CSQL *aSQL)
{
    if (!aSQL)
       return;

    if (!mRoot)
    {
        mRoot = mTail = aSQL;
    }    
    else
    {
        mTail->Next = aSQL;
        mTail = aSQL;
    }
    mLength++;    
}    

CSQL *CStoreCenter::pop()
{
    if (!mRoot)
        return NULL;
    CSQL *Ret = mRoot;
    mRoot = mRoot->Next;
    mLength--;
    return Ret;
}    
bool CStoreCenter::query (const char *aTable, const char *aQuery)
{
        int j;
       for (j=0; mDatabaseConnection->query(aQuery) && j < 3; j++)
       {
           system_log("retrying failed query!");
       }
       if (j != 3)
       {
          return true;
       }    
       else
       {
           system_log("failed: %s", aQuery);
       }    
       return false;
}

bool CStoreCenter::is_empty()
{
  if (mRoot == NULL)
      return true;
  return false;   
}    
bool
CStoreCenter::store(CStore &aStore)
{
    CSQL *SQL = new CSQL();
    SQL->set_query(aStore.query());
	push(SQL);
	aStore.type(QUERY_NONE);
	return true;
}

int
CStoreCenter::write()
{
    static int count = 0;
    static int failed = 0;
    static time_t start = time(0);

    // Return codes are:
    // -1 -- empty/idle
    // 0 -- wrote/ran
    // 1 -- error
    
	if (is_empty())
	{
	    lock = false;
	    return -1;
	}

	// cannot pop a NULL pointer in this case
	CSQL *aSQL = pop();
	
    int j;
    for (j=0; mDatabaseConnection->query(aSQL->get_query()) && j < 3; j++)
    {
      system_log("retrying failed query #%d!", j+1);
    }
    delete aSQL;;    
    if (j != 3)
    {
      count++;
    }    
    else
    {
      failed++;
      lock = false;
      return 1;
    }    

	if (time(0)-start >= 60)
	{
	  system_log("DATABASE REPORT : TOTALS good queries = %d failed queries = %d, time period = %d, length = %d ", count, failed, time(0)-start, length());
	  start = time(0);
	  count = 0;
	  failed = 0;
	}

	lock = false;
	return 0;
}
