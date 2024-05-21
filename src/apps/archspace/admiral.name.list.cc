#include "admiral.h"

CAdmiralNameList::CAdmiralNameList():CStringList(100, 100)
{
}

CAdmiralNameList::~CAdmiralNameList()
{
	remove_all();
}

bool
CAdmiralNameList::add_name(char *aName)
{
	if (!aName) return false;

	int
		Index = find_sorted_key((TSomething)aName);
	if (Index >= 0) return false;

	insert_sorted(aName);

	return true;
}
