#include "tech.h"

CTechTable::CTechTable()
{
}

CTechTable::~CTechTable()
{
	remove_all();
}

bool
CTechTable::free_item(TSomething aItem)
{
	CTech
		*Tech = (CTech*)aItem;

	if (!Tech) return false;

	delete Tech;

	return true;
}

int
CTechTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CTech
		*Tech1 = (CTech*)aItem1,
		*Tech2 = (CTech*)aItem2;

	if (Tech1->get_id() > Tech2->get_id()) return 1;
	if (Tech1->get_id() < Tech2->get_id()) return -1;
	return 0;
}

int
CTechTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CTech
		*Tech = (CTech*)aItem;

	if (Tech->get_id() > (int)aKey) return 1;
	if (Tech->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CTechTable::remove_tech(int aTechID)
{
	int
		Index;

	Index = find_sorted_key((void*)aTechID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

int
CTechTable::add_tech(CTech *aTech)
{
	if (!aTech) return -1;

	if (find_sorted_key((TConstSomething)aTech->get_id()) >= 0)
		return -1;
	insert_sorted(aTech);

	return aTech->get_id();
}

CTech*
CTechTable::get_by_id(int aTechID)
{
	if (!length()) return NULL;

	int
		Index;

	Index = find_sorted_key((void*)aTechID);

	if (Index < 0) return NULL;

	return (CTech*)get(Index);
}
CTech* CTechTable::get_by_name(const char* name)
{
	for(int i=0 ; i<length() ; i++)
	{
		CTech*
			tech = (CTech*)get(i);
		if( strcmp(name ,tech->get_name() ) == 0)
		{
			return tech;
		}
	}
	return NULL;
}
// start telecard 2001/02/16
CTechList*
CTechTable::get_by_level(int aLevel)
{
	CTechList*
		techList = new CTechList();
	for(int i=0 ; i<length() ; i++)
	{
		CTech*
			tech = (CTech*)get(i);
		if( tech->get_level() == aLevel )
		{
			techList->add_tech( tech );
		}
	}
	return techList;
}
// end telecard 2001/02/16
