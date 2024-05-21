#include "portal.h"
#include "archspace.h"
#include <cstdlib>

CPortalUserTable::CPortalUserTable()
{
}

CPortalUserTable::~CPortalUserTable()
{
	remove_all();
}

bool
CPortalUserTable::free_item(TSomething aItem)
{
	CPortalUser
		*PortalUser = (CPortalUser*)aItem;
	if (!PortalUser) return false;
	delete PortalUser;
	return true;
}

int 
CPortalUserTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CPortalUser
		*PortalUser1 = (CPortalUser*)aItem1,
		*PortalUser2 = (CPortalUser*)aItem2;
	if (PortalUser1->get_portal_id() < PortalUser2->get_portal_id()) return -1;
	if (PortalUser1->get_portal_id() > PortalUser2->get_portal_id()) return 1;
	return 0;
}

int 
CPortalUserTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPortalUser
		*PortalUser = (CPortalUser*)aItem;
	if (PortalUser->get_portal_id() < (int)aKey) return -1;
	if (PortalUser->get_portal_id() > (int)aKey) return 1;
	return 0;
}

bool
CPortalUserTable::load(CMySQL &aMySQL)
{
    char *PortalDB = CONFIG->get_string("Database", "PortalUserDB", NULL);
	char *PortalTable = CONFIG->get_string("Database", "PortalUserTable", NULL);
	if (PortalDB == NULL)
	   PortalDB = "Archspace";
 	if (PortalTable == NULL)
 	   PortalTable = "asbb_users"; 

	SLOG( "PortalUser loading" );
	CString
		Query;
//    Query.clear();
//    Query.format( "LOCK TABLE %s.%s READ", PortalDB, PortalTable);
//	aMySQL.query( (char*)Query );
    Query.clear();           
	Query.format( "SELECT "
			"user_level, "
			"dev_level, "
			"username, "
			"user_id, "
			"user_password, "
			"user_email, "
			"age, "
			"country, "
			"firstlogin, "
			"ip "               
			"FROM %s.%s where user_id > 0", PortalDB, PortalTable);
	aMySQL.query( (char*)Query );
	aMySQL.use_result();
 	while(aMySQL.fetch_row())
	{
	
		add_user(new CPortalUser(aMySQL.row()));
	}	
	
	aMySQL.free_result();

//	aMySQL.query( "UNLOCK TABLES" );
	return true;
}

bool
CPortalUserTable::load_new_by_id(int aPortalID)
{
	char *PortalTable = CONFIG->get_string("Database", "PortalUserTable", NULL);
 	if (PortalTable == NULL)
 	   PortalTable = "Users"; 

	SLOG( "adding new portal user to PORTAL_USER table" );
	CMySQL
		aMySQL;
	char
		*Host = CONFIG->get_string("Database", "Host", NULL),
		*User = CONFIG->get_string("Database", "User", NULL),
		*Password = CONFIG->get_string("Database", "Password", NULL),
		*Database = CONFIG->get_string("Database", "PortalUserDB", NULL);
	if (Database == NULL)
        Database = 	"Archspace";
	if (Host == NULL || User == NULL || Password == NULL || Database == NULL) return false;

	if (aMySQL.open(Host, User, Password, Database) == false) return false;

	CString
		Query;
//    Query.clear();
//    Query.format( "LOCK TABLE %s READ", PortalTable);
//	aMySQL.query( (char*)Query );
    Query.clear();           

	Query.format( "SELECT "
			"user_level, "
			"dev_level, "
			"username, "
			"user_id, "
			"user_password, "
			"user_email, "
			"age, "
			"country, "
			"firstlogin, "
			"ip "               
			"FROM %s where user_id=%d", PortalTable, aPortalID);

	aMySQL.query( (char*)Query );
	aMySQL.use_result();

 	while(aMySQL.fetch_row())
	{
		add_user(new CPortalUser(aMySQL.row()));
	}
	
	aMySQL.free_result();

//	aMySQL.query( "UNLOCK TABLES" );
    aMySQL.close();
	return true;
}


CPortalUser*
CPortalUserTable::get_by_portal_id(int aPortalID)
{
    if (!length()) 
       return NULL;
	int Index;
    Index = find_sorted_key((void*)aPortalID);
	if (Index < 0) 
       return NULL;
	CPortalUser
		*PortalUser = (CPortalUser*)get(Index);
    if (!PortalUser)
       return NULL;
	return PortalUser;
}

int
CPortalUserTable::add_user(CPortalUser *aPortalUser)
{
	if (!aPortalUser) return -1;

	if (find_sorted_key((TConstSomething)aPortalUser->get_portal_id()) >= 0) return -1;
	insert_sorted(aPortalUser);

	return aPortalUser->get_portal_id();
}

bool
CPortalUserTable::remove_user(int aPortalID)
{
    int Index;
    
    Index = find_sorted_key((void*)aPortalID);
    if (Index < 0) return false;

	return CSortedList::remove(Index);
}
