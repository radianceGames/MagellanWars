#if !defined(__ARCHSPACE_STORE_H__)
#define __ARCHSPACE_STORE_H__

#include "common.h"
#include "util.h"
#include "net.h"
#include <string>
#include <queue>
using namespace std;

extern TZone gSQLZone;
extern TZone gStoreCenterZone;

#define MESSAGE_SQL_SEND		0x7001
#define MESSAGE_PUSH_REQUEST	0x7003
#define QUERY_NONE 				0
#define QUERY_INSERT	 		1
#define QUERY_UPDATE			2
#define QUERY_DELETE			3

/**
*/
class CStore: public CBase
{
	public:
		CStore();
		virtual ~CStore();

		virtual const char *table() = 0;

		virtual CString &query() = 0;

		int type(int aType = -1);

		bool is_changed();

	/* For Test */
	public:
		bool set_all_changed();

	protected:
		int
			mQueryType;
		CCommandSet
			mStoreFlag;
};

/**
*/
class CSQL
{
	public:
		CSQL();
		~CSQL();

		void set_query(const char *aQuery);
		const char* get_query() { return mQuery.c_str(); }
		CSQL *Next;
	private:
		string mQuery;
	RECYCLE(gSQLZone);
};

class CStoreCenter
{
	public:
		CStoreCenter();
		virtual ~CStoreCenter();

		bool initialize(CMySQL *aMySQL);
		bool store(CStore &aStore);
		bool query (const char *aTable, const char *aQuery);		
		void push(CSQL *aSQL);
		CSQL *pop();
		bool is_empty();
		virtual int write();
		inline CMySQL *get_database_connection() { return mDatabaseConnection; } 		
		inline bool is_locked() { return lock; }
		inline int length() { return mLength; }
	private:
        CMySQL *mDatabaseConnection;	
        CSQL *mRoot;
        CSQL *mTail;
        bool lock;
        int mLength;
	protected:
		virtual const char *debug_info() const { return "store center"; }

	RECYCLE(gStoreCenterZone);
};

class CMySQLPool
{
      public:
        CMySQLPool();
        virtual ~CMySQLPool();
        
        bool initialize(char *Host, char *User, char *Password, char *Database, int numConnections);
        CMySQL *get_connection();
        bool release_connection(CMySQL *aMySQL);
        bool close();
        
      private:
        queue<CMySQL *> mPool;
        int mMaxConnections;
};

#endif
