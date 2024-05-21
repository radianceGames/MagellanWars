#include "action.h"

CActionList::CActionList():CSortedList(10, 10)
{
}

CActionList::~CActionList()
{
	remove_all();
}

bool
CActionList::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

int
CActionList::compare(TSomething aItem1, TSomething aItem2) const
{
	CAction
		*Action1 = (CAction*)aItem1,
		*Action2 = (CAction*)aItem2;

	assert(Action1);
	assert(Action2);

	if (Action1->get_id() > Action2->get_id()) return 1;
	if (Action1->get_id() < Action2->get_id()) return -1;
	return 0;
}

int
CActionList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CAction
		*Action = (CAction*)aItem;

	assert(Action);

	if (Action->get_id() > (int)aKey) return 1;
	if (Action->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CActionList::remove_action(int aActionID)
{
	int 
		Index;

	Index = find_sorted_key((void*)aActionID);

	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

bool 
CActionList::add_action(CAction* aAction)
{
	assert(aAction);

	if (find_sorted_key((TConstSomething)aAction->get_id()) >= 0) 
		return false;
	
	insert_sorted(aAction);

	return true;
}

CAction*
CActionList::get_by_id(int aActionID)
{
	if (!length()) return NULL;

	int 
		Index;

	Index = find_sorted_key((void*)aActionID);

	if (Index < 0) return NULL;

	return (CAction*)get(Index);
}


