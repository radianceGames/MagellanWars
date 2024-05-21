#include "relation.h"
#include "council.h"

CCouncilRelationList::CCouncilRelationList()
{
	mOwner = NULL;
}

CCouncilRelationList::~CCouncilRelationList()
{
	remove_all();
}

void
CCouncilRelationList::set_owner(CCouncil *aOwner)
{
	mOwner = aOwner;
}

CCouncilRelation*
CCouncilRelationList::find_by_target_id(int aID)
{
	for(int i=0; i<length(); i++)
	{
		CCouncilRelation* Relation = (CCouncilRelation*)get(i);
		if (Relation->get_council1() == mOwner) 
		{
			if (Relation->get_council2()->get_id() == aID) 
				return Relation;
		} else {
			if (Relation->get_council1()->get_id() == aID) 
				return Relation;
		}
	}

	return NULL;
}

bool 
CCouncilRelationList::remove_council_relation_by_target_id(int aTargetID)
{
	CCouncilRelation* PRelation = NULL; 

	for(int i=0; i<length(); i++)
	{
		CCouncilRelation* Relation = (CCouncilRelation*)get(i);
		if (Relation->get_council1() == mOwner) 
		{
			if (Relation->get_council2()->get_id() == aTargetID)
			{
				PRelation = Relation;
				break;
			}
		} else {
			if (Relation->get_council1()->get_id() == aTargetID)
			{
				PRelation = Relation;
				break;
			}
		}
	}

	if (!PRelation) return false;

	return remove_relation(PRelation->get_id());
}
