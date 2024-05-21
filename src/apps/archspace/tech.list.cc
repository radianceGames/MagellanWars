#include "tech.h"

CTechList::CTechList()
{
}

CTechList::~CTechList()
{
	remove_all();
}

bool
CTechList::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

int
CTechList::compare(TSomething aItem1, TSomething aItem2) const
{
	CTech
		*Tech1 = (CTech *)aItem1,
		*Tech2 = (CTech *)aItem2;

	if (Tech1->get_id() > Tech2->get_id()) return 1;
	if (Tech1->get_id() < Tech2->get_id()) return -1;
	return 0;
}

int
CTechList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CTech
		*Tech = (CTech *)aItem;

	if (Tech->get_id() > (int)aKey) return 1;
	if (Tech->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CTechList::remove_tech(int aTechID)
{
	int 
		Index;

	Index = find_sorted_key((void*)aTechID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

int 
CTechList::add_tech(CTech *aTech)
{
	if (!aTech) return -1;

	if (find_sorted_key((TConstSomething)aTech->get_id()) >= 0) 
		return -1;
	insert_sorted(aTech);

	return aTech->get_id();
}

CTech *
CTechList::get_by_id(int aTechID)
{
	if (!length()) return NULL;

	int 
		Index;

	Index = find_sorted_key((void*)aTechID);

	if (Index < 0) return NULL;

	return (CTech *)get(Index);
}

CTech *
CTechList::get_by_level( int aLevel )
{
	for( int i = 0; i < length(); i++ ){
		CTech
			*Tech = (CTech*)get(i);

		if( Tech->get_level() == aLevel ) return Tech;
	}

	return NULL;
}

