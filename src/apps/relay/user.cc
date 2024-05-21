#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "data.h"

CUser::CUser():CStore()
{
	mGameID = -1;
	mPortalID = -1;

	mServer = -1;
	mRace = -1;

	mCreateTime = 0;
	mLastLogin = 0;
}

CUser::CUser(MYSQL_ROW aRow):CStore()
{
	mGameID = atoi(aRow[0]);
	mPortalID = atoi(aRow[1]);
	mName = (char*)aRow[2];
	mRace = atoi(aRow[3]);
	mCreateTime = atoi(aRow[4]);
	mLastLogin = atoi(aRow[5]);
	mServer = atoi(aRow[6]);
}

CUser::~CUser()
{
}

CString&
CUser::query()
{
	static CString
		Query;

	Query.clear();

	switch(type())
	{
		case QUERY_INSERT:
			Query = "INSERT INTO User "
					"(GameID, PortalID, Name, Race, CreateTime, "
					"LastLogin, Server) ";
			Query.format(
					"VALUES (%d, %d, \"%s\", %d, %d, %d, %d)",
					mGameID, 
					mPortalID, 
					(char*)mName, 
					mRace, 
					mCreateTime,
					mLastLogin,
					mServer);
			break;
		case QUERY_UPDATE:
			Query = "UPDATE User SET ";
			Query.format("LastLogin = %d, Server = %d "
						"WHERE GameID = %d", 
					mLastLogin, mServer, mGameID);
			break;
	}

	return Query;
}

bool
CUser::create(int aGameID, int aPortalID, const char *aName,
				int aRace, int aServer, time_t aCreateTime)
{
	mGameID = aGameID;
	mPortalID = aPortalID;
	mName = aName;
	mRace = aRace;
	mServer = aServer;
	mCreateTime = aCreateTime;
	mLastLogin = time(0);

	return true;
}

CStoreCenter& 
operator<<(CStoreCenter &aStoreCenter, CUser &aUser)
{
	system_log("save user %s", aUser.get_name());

	aStoreCenter.store(aUser);

	return aStoreCenter;
}
