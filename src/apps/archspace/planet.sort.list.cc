#include "planet.h"
#include "archspace.h"

CSortedPlanetList::CSortedPlanetList(): CSortedList(10, 10)
{
}

CSortedPlanetList::~CSortedPlanetList()
{
	remove_all();
}

bool
CSortedPlanetList::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

int
CSortedPlanetList::compare(TSomething aItem1, TSomething aItem2) const
{
	assert(aItem1);
	assert(aItem2);

	CPlanet
		*Planet1 = (CPlanet*)aItem1,
		*Planet2 = (CPlanet*)aItem2;

	if (Planet1->get_id() > Planet2->get_id()) return 1;
	if (Planet1->get_id() < Planet2->get_id()) return -1;

	return 0;
}

int
CSortedPlanetList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	assert(aItem);

	CPlanet
		*Planet = (CPlanet*)aItem;

	if (Planet->get_id() > (int)aKey) return 1;
	if (Planet->get_id() < (int)aKey) return -1;

	return 0;
}

int
CSortedPlanetList::add_planet(CPlanet* aPlanet)
{
	assert(aPlanet);

	if (find_sorted_key((TConstSomething)aPlanet->get_id()) >=0 ) 
		return -1;

	insert_sorted(aPlanet);

	return aPlanet->get_id();
}

bool
CSortedPlanetList::remove_planet(int aPlanetID)
{
	int 
		Index = find_sorted_key((TConstSomething)aPlanetID);

	if (Index < 0) return false;

	return CSortedList::remove(Index);
}

CPlanet *
CSortedPlanetList::get_by_id(int aPlanetID)
{
	int
		Index = find_sorted_key((void *)aPlanetID);
	if (Index < 0) return NULL;

	return (CPlanet *)get(Index);
}

bool
CSortedPlanetList::update_DB()
{
	for (int i=0 ; i<length() ; i++)
	{
		CPlanet *
			Planet = (CPlanet *)get(i);
		if (Planet->is_changed() == false) continue;

		Planet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Planet);
	}

	return true;
}

