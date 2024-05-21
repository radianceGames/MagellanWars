#include "store.h"
#include "cgi.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

TZone gCMySQLPoolZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CMySQLPool),
	0,
	0,
	NULL,   
	"Zone CMySQLPool"
};

CMySQLPool::CMySQLPool()
{
                        
}

CMySQLPool::~CMySQLPool()
{
      // this may be irrelvant and handled by the zone (or even cause problems)
     while (!mPool.empty())
      {
            CMySQL *aMySQL = mPool.front();
            mPool.pop();
            aMySQL->close();
//            delete aMySQL;            
      }                   
}

bool
CMySQLPool::initialize(char *Host, char *User, char *Password, char *Database, int numConnections)
{
  if (numConnections <= 0)
     return false;
  mMaxConnections = numConnections;
  if (Host == NULL || User == NULL || Password == NULL || Database == NULL) 
     return false;  
  
  for (int i=0; i < numConnections; i++)
  {
      CMySQL *aMySQL = new CMySQL();
      aMySQL->open(Host, User, Password, Database);
      mPool.push(aMySQL);
  }                      
  return true;
}

bool CMySQLPool::release_connection(CMySQL *aMySQL)
{
//    system_log("release start size: %d", mPool.size());		
    if ((int)mPool.size() >= mMaxConnections+1 || aMySQL == NULL)
       return false;

//    if (!aMySQL->has_result())
//        aMySQL->use_result();
        
//    if (aMySQL->has_result())
//    {
//	  while (aMySQL->fetch_row())
//		;
//	  aMySQL->free_result();
//    }

    mPool.push(aMySQL);
//	system_log("release end end size: %d", mPool.size());
    return true;
}

bool CMySQLPool::close()
{
   if (mPool.empty())
      return false;
      
   CMySQL *aMySQL = NULL;
   while ((aMySQL = mPool.front()))
   {
         aMySQL->close();
         mPool.pop();              
   }
   
   return true;  
}

CMySQL * CMySQLPool::get_connection()
{
// system_log("get connection start size: %d", mPool.size());
 if (mPool.empty())
    return NULL;      
 CMySQL *aMySQL = mPool.front();
 mPool.pop();
 
// if (!aMySQL->has_result())
//    aMySQL->use_result();
// if (aMySQL->has_result())
//   {
//	while (aMySQL->fetch_row())
//		;
//	aMySQL->free_result();
//   }
 //system_log("get connection end size: %d", mPool.size());
 return aMySQL;
}
