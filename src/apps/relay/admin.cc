#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "data.h"

CAdmin::CAdmin()
{
	mPortalID = -1;

	mName = "";
	mAuthority = "";
}

CAdmin::CAdmin(MYSQL_ROW aRow)
{
	mPortalID = atoi(aRow[0]);
	mName = aRow[1];
	mAuthority = aRow[2];
}

CAdmin::~CAdmin()
{
}

CString&
CAdmin::query()
{
	static CString
		Query;

	Query.clear();

	switch(type())
	{
		case QUERY_INSERT:
			Query = "INSERT INTO Admin "
					"(PortalID, Name, Authority) "
					"VALUE ";
			Query.format("(%d, \"%s\", \"%s\")",
					mPortalID,  
					(char*)mName, (char*)mAuthority);
			break;
		case QUERY_UPDATE:
			Query = "UPDATE Admin SET ";
			Query.format("Authority = \"%s\" WHERE PortalID = %d",
					(char*)mAuthority, mPortalID);
			break;
		case QUERY_DELETE:
			Query = "DELETE FROM Admin WHERE ";
			Query.format("PortalID = %d", mPortalID);
	}

	return Query;
}

bool
CAdmin::initialize(int aPortalID,  
		char *aName, char *aAuthority)
{
	mPortalID = aPortalID;
	mName = aName;
	mAuthority = aAuthority;

	return true;
}
