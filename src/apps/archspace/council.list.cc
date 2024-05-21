#include "council.h"

CCouncilList::~CCouncilList()
{
	remove_all();
}

bool
CCouncilList::free_item(void *aItem)
{
	(void)aItem;

	return true;
}

CCouncil*
CCouncilList::get_by_id(int aID)
{
	int
		Index = find_sorted_key((TConstSomething)aID);
	if (Index < 0) return NULL;
	return (CCouncil*)get(Index);
}

int
CCouncilList::compare(TSomething aItem1, TSomething aItem2) const
{
	CCouncil
		*Council1 = (CCouncil*)aItem1,
		*Council2 = (CCouncil*)aItem2;

	if (Council1->get_id() > Council2->get_id()) return 1;
	if (Council1->get_id() < Council2->get_id()) return -1;

	return 0;
}

int
CCouncilList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CCouncil
		*Council = (CCouncil*)aItem;

	if (Council->get_id() > (int)aKey) return 1;
	if (Council->get_id() < (int)aKey) return -1;

	return 0;
}

int
CCouncilList::add_council(CCouncil *aCouncil)
{
	assert(aCouncil);

	int
		Index = find_sorted_key((TConstSomething)aCouncil->get_id());
	if (Index >= 0) return -1;

	insert_sorted(aCouncil);

	return aCouncil->get_id();
}

bool 
CCouncilList::remove_council(int aCouncil)
{
	int
		Index = find_sorted_key((TConstSomething)aCouncil);

	if (Index < 0) return false;

	return CSortedList::remove(Index);
}
