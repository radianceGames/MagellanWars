#include "portal.h"

TZone gPortalUserListZone =
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CPortalUserList),
	0,
	0,
	NULL,
	"Zone CPortalUserList"
};

CPortalUserList::CPortalUserList()
{
}

CPortalUserList::~CPortalUserList()
{
	remove_all();
}

bool
CPortalUserList::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

int
CPortalUserList::compare(TSomething aItem1, TSomething aItem2) const
{
	CPortalUser
		*PortalUser1 = (CPortalUser*)aItem1,
		*PortalUser2 = (CPortalUser*)aItem2;

	if (PortalUser1->get_portal_id() > PortalUser2->get_portal_id()) return 1;
	if (PortalUser1->get_portal_id() < PortalUser2->get_portal_id()) return -1;
	return 0;
}

int
CPortalUserList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CPortalUser
		*PortalUser = (CPortalUser*)aItem;

	if (PortalUser->get_portal_id() > (int)aKey) return 1;
	if (PortalUser->get_portal_id() < (int)aKey) return -1;
	return 0;
}

bool
CPortalUserList::remove_user(int aPortalID)
{
	int Index = find_sorted_key((void *)aPortalID);	
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

bool
CPortalUserList::add_user(CPortalUser* aPortalUser)
{
	if (!aPortalUser) return false;

	if (find_sorted_key((TConstSomething)aPortalUser->get_portal_id()) >= 0) 
		return false;
	insert_sorted(aPortalUser);

	return true;
}

CPortalUser*
CPortalUserList::get_by_portal_id(int aPortalID)
{
	if (!length()) return NULL;

	int Index = find_sorted_key((void*)aPortalID);

	if (Index < 0) return NULL;

	return (CPortalUser *)get(Index);
}
