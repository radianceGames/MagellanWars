#include "relation.h"

CRelationList::CRelationList():CSortedList(10, 10)
{
}

CRelationList::~CRelationList()
{
	remove_all();
}

bool
CRelationList::free_item(TSomething aItem)
{
	(void)aItem;
	return true;
}

int
CRelationList::compare(TSomething aItem1, TSomething aItem2) const
{
	CRelation *Relation1, *Relation2;

	Relation1 = (CRelation*)aItem1;
	Relation2 = (CRelation*)aItem2;

	assert(Relation1);
	assert(Relation2);

	if (Relation1->get_id() < Relation2->get_id()) return -1;
	if (Relation1->get_id() > Relation2->get_id()) return 1;

	assert(Relation1->get_id() != Relation2->get_id());

	return 0;
}

int 
CRelationList::compare_key(TSomething aItem, TConstSomething aKey) const
{
	CRelation *Relation;

	Relation = (CRelation*)aItem;

	assert(Relation);

	if (Relation->get_id() < (int)aKey) return -1;
	if (Relation->get_id() > (int)aKey) return 1;

	return 0;
}

CRelation*
CRelationList::find_by_id(int aID)
{
	int Index;

	Index = find_sorted_key((TConstSomething)aID);

	if (Index < 0) return NULL;

	return (CRelation*)get(Index);
}

bool
CRelationList::is_there_relation(int aRelation)
{
	bool
		AnyRelation = false;

	for (int i=0 ; i<length() ; i++)
	{
		CRelation *
			Relation = (CRelation *)get(i);

		if (Relation->get_relation() != aRelation) continue;

		AnyRelation = true;
		break;
	}

	return AnyRelation;
}

bool
CRelationList::add_relation(CRelation *aRelation)
{
	int Index;

	assert(aRelation);

	Index = find_sorted_key((TConstSomething)aRelation->get_id());

	if (Index >= 0) return false;
	
	insert_sorted(aRelation);

	return true;
}

bool 
CRelationList::remove_relation(int aID)
{
	int
		Index = find_sorted_key((TConstSomething)aID);
	if (Index < 0) return false;

	return CSortedList::remove(Index);
}
