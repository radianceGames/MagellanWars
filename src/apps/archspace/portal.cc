#include "portal.h"
#include "archspace.h"
TZone gPortalUserZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPortalUser),
	0,
	0,
	NULL,
	"Zone CPortalUser"
};

char *user_level_to_string(EUserLevel aUserLevel)
{
 switch (aUserLevel)
 {
   case UL_BANNED: return "BANNED";
   case UL_PLAYER: return "PLAYER";
   case UL_MODERATOR: return "MODERATOR";
   case UL_ADMIN: return "ADMINISTRATOR";
   case UL_DEVELOPER: return "DEVELOPER";
   default:  return "NONE";
 };
}

char *dev_level_to_string(EDevLevel aDevLevel)
{
 switch (aDevLevel)
 {
   case DL_NONE:  return "NONE";
   case DL_BALANCE:  return "BALANCING";
   case DL_CREATIVE:  return "CREATIVE";
   case DL_IMAGE:  return "IMAGING";
   case DL_ALL:  return "ALL";   
   default:  return "NONE";
 };
}

EUserLevel string_to_user_level(char *str)
{
   if (str == NULL)
       return UL_PLAYER;
   if (!strcmp(user_level_to_string(UL_BANNED),str))
       return UL_BANNED;
   if (!strcmp(user_level_to_string(UL_PLAYER),str))
       return UL_PLAYER;
   if (!strcmp(user_level_to_string(UL_MODERATOR),str))
       return UL_MODERATOR;
   if (!strcmp(user_level_to_string(UL_ADMIN),str))
       return UL_ADMIN;
   if (!strcmp(user_level_to_string(UL_DEVELOPER),str))
       return UL_DEVELOPER;
   return UL_PLAYER;  // todo: use a default level
}

EDevLevel string_to_dev_level(char *str)
{
   if (str == NULL)
       return DL_NONE;
   if (!strcmp(dev_level_to_string(DL_BALANCE),str))
       return DL_BALANCE;
   if (!strcmp(dev_level_to_string(DL_CREATIVE),str))
       return DL_CREATIVE;
   if (!strcmp(dev_level_to_string(DL_IMAGE),str))
       return DL_IMAGE;
   if (!strcmp(dev_level_to_string(DL_ALL),str))
       return DL_ALL;
   return DL_NONE;   // todo: use a  default level
}



CPortalUser::CPortalUser(MYSQL_ROW aRow)
{
    mPortalID = atoi(aRow[STORE_ID]);
    mUserLevel = (EUserLevel)atoi(aRow[STORE_USER_LEVEL]);
    mDevLevel = (EDevLevel)atoi(aRow[STORE_DEV_LEVEL]);
    mAge = atoi(aRow[STORE_AGE]);
    mName = aRow[STORE_NAME];
    mPassword = aRow[STORE_PASSWORD];
    mEmail = aRow[STORE_EMAIL];        
    mCountry = aRow[STORE_COUNTRY];
    mFirstLogin = aRow[STORE_FIRST_LOGIN];    
    mIP = aRow[STORE_IP];        

	mPrev = NULL;
	mNext = NULL;
   	type(QUERY_NONE);
}

CPortalUser::~CPortalUser()
{

}

CString&
CPortalUser::query()
{
    char *PortalDB = CONFIG->get_string("Database", "PortalUserDB", NULL);
	char *PortalTable = CONFIG->get_string("Database", "PortalUserTable", NULL);
	if (PortalDB == NULL)
	   PortalDB = "Archspace";
 	if (PortalTable == NULL)
 	   PortalTable = "asbb_users"; 
	static CString 
		Query;

	Query.clear();

	switch(CStore::type())
	{
		case QUERY_INSERT :
			Query.format("INSERT INTO %s.%s"
				" (user_id, user_level, dev_level, username, user_password,"
				" user_age, user_email, country, firstlogin,"
				" ip)"
				" VALUES (%d, '%d', %d, %s,"
				" %s, %s, %s, %s, %s, %s)",
				        PortalDB,
                        PortalTable,
						mPortalID,
                        mUserLevel,
                        mDevLevel,
						(char*)add_slashes((char *)mName), 
   						(char*)add_slashes((char *)mPassword), 
   						(char*)add_slashes((char *)mAge), 
   						(char*)add_slashes((char *)mEmail), 
   						(char*)add_slashes((char *)mCountry),                            
						(char*)add_slashes((char *)mFirstLogin), 
						(char*)add_slashes((char *)mIP)); 
			break;

		case QUERY_UPDATE :
			Query.format("UPDATE %s.%s SET user_level = %d", PortalDB, PortalTable, (int)mUserLevel);

#define STORE(x, y, z) \
			if (mStoreFlag.has(x)) \
				Query.format(y, z)

			STORE(STORE_DEV_LEVEL, ", dev_level = %d", (int)mDevLevel);
			STORE(STORE_AGE, ", age = '%d'", mAge);
         
			if (mName.length() > 0)
			{
				STORE(STORE_NAME, ", username = '%s'",
						(char *)add_slashes((char *)mFirstLogin));
			}

			if (mPassword.length() > 0)
			{
				STORE(STORE_PASSWORD, ", user_password = '%s'",
						(char *)add_slashes((char *)mPassword));
			}

			if (mEmail.length() > 0)
			{
				STORE(STORE_EMAIL, ", user_email = '%s'",
						(char *)add_slashes((char *)mEmail));
			}


			if (mCountry.length() > 0)
			{
				STORE(STORE_COUNTRY, ", country = '%s'",
						(char *)add_slashes((char *)mCountry));
			}


			if (mFirstLogin.length() > 0)
			{
				STORE(STORE_FIRST_LOGIN, ", firstlogin = '%s'",
						(char *)add_slashes((char *)mFirstLogin));
			}


			if (mIP.length() > 0)
			{
				STORE(STORE_IP, ", ip = '%s'",
						(char *)add_slashes((char *)mIP));
			}

            Query.format(" WHERE user_id = %d", mPortalID);

			break;

		case QUERY_DELETE :
			Query.format("DELETE FROM %s.%s WHERE user_id = %d", 
                        PortalDB,
                        PortalTable,
						mPortalID );
			break;
	}

	mStoreFlag.clear();

	return Query;
}

