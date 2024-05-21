#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "data.h"

CAdminTable::CAdminTable(): CSortedList(10, 10)
{
}

CAdminTable::~CAdminTable()
{
	remove_all();
}

bool
CAdminTable::free_item(TSomething aItem)
{
	CAdmin 
		*Admin = (CAdmin*)aItem;

	if (!Admin) return false;

	delete Admin;

	return true;
}

int
CAdminTable::compare(TSomething aItem1, TSomething aItem2, 
			int aMode) const
{
	(void)aMode;
	CAdmin
		*Admin1 = (CAdmin*)aItem1,
		*Admin2 = (CAdmin*)aItem2;

	if (Admin1->get_portal_id() > Admin2->get_portal_id()) return 1;
	if (Admin1->get_portal_id() < Admin2->get_portal_id()) return -1;
	return 0;
}

int
CAdminTable::compare_key(TSomething aItem, TConstSomething aKey, 
			int aMode) const
{
	(void)aMode;
	CAdmin
		*Admin = (CAdmin*)aItem;

	if (Admin->get_portal_id() > (int)aKey) return 1;
	if (Admin->get_portal_id() < (int)aKey) return -1;
	return 0;
}

const char *
CAdminTable::has(int aPortalID)
{
	int 
		Pos = find_sorted_key((TSomething)aPortalID);

	if (Pos < 0) 
		return NULL;	

	CAdmin 
		*Admin = (CAdmin*)get(Pos);

	return Admin->get_authority();
}

bool
CAdminTable::load(CMySQL &aMySQL)
{
	aMySQL.query("LOCK TABLE Admin READ");

	aMySQL.query("SELECT PortalID, Name, Authority "
					"FROM Admin");

	aMySQL.use_result();
	while(aMySQL.fetch_row())
		insert_sorted(new CAdmin(aMySQL.row()));
	aMySQL.free_result();

	aMySQL.query("UNLOCK TABLES");

	return true;
}

