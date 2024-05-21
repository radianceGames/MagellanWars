#include "race.h"

TZone gRaceTableZone = 
{
	PTH_MUTEX_INIT,
	recycle_allocation,
	recycle_free,
	sizeof(CRaceTable),
	0,
	0,
	NULL,
	"Zone CRaceTable"
};

CRaceTable::CRaceTable()
{
}

CRaceTable::~CRaceTable()
{
	remove_all();
}

bool
CRaceTable::free_item(TSomething aItem)
{
	CRace
		*Race = (CRace*)aItem;

	if (!Race) return false;

	delete Race;

	return true;
}

int
CRaceTable::compare(TSomething aItem1, TSomething aItem2) const
{
	CRace
		*Race1 = (CRace*)aItem1,
		*Race2 = (CRace*)aItem2;

	if (Race1->get_id() > Race2->get_id()) return 1;
	if (Race1->get_id() < Race2->get_id()) return -1;
	return 0;
}

int
CRaceTable::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CRace
		*Race = (CRace*)aItem;

	if (Race->get_id() > (int)aKey) return 1;
	if (Race->get_id() < (int)aKey) return -1;
	return 0;
}

bool
CRaceTable::remove_race(int aRaceID)
{
	int 
		Index;

	Index = find_sorted_key((void*)aRaceID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

int 
CRaceTable::add_race(CRace *aRace)
{
	if (!aRace) return -1;

	if (find_sorted_key((TConstSomething)aRace->get_id()) >= 0) 
		return -1;
	insert_sorted(aRace);

	return aRace->get_id();
}

CRace*
CRaceTable::get_by_id(int aRaceID)
{
	if (!length()) return NULL;

	int 
		Index;

	Index = find_sorted_key((void*)aRaceID);

	if (Index < 0) return NULL;

	return (CRace*)get(Index);
}

const char *
CRaceTable::get_name_by_id(int aRaceID)
{
	if (aRaceID<1 || aRaceID > MAX_RACE) return NULL;
	return get_by_id(aRaceID)->get_name();
}

int
CRaceTable::get_id_by_name(char *aName)
{
	if (!aName) return -1;

	for (int i=0 ; i<length() ; i++)
	{
		if (!strcmp(((CRace *)get(i))->get_name(), aName)) return i+1;
	}
		
	return -1;
}

