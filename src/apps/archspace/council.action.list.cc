#include "action.h"

CCouncilActionList::~CCouncilActionList()
{
	remove_all();
}

bool 
CCouncilActionList::add_action(CCouncilAction* aAction)
{
	assert(aAction);

	if (find_sorted_key((TConstSomething)aAction->get_id()) >= 0) 
		return false;
	
	insert_sorted(aAction);

	return true;
}


CCouncilAction* 
CCouncilActionList::get_declare_total_war_action(int mCouncilID)
{
	for(int i=0; i<length(); i++)
	{
		CCouncilAction *Action = (CCouncilAction*)get(i);
		if ((Action->get_type() == CAction::ACTION_COUNCIL_DECLARE_TOTAL_WAR)
					&& (Action->get_argument() == mCouncilID))	
			return Action;
	}
	return NULL;
}

CCouncilAction* 
CCouncilActionList::get_declare_war_action(int mCouncilID)
{
	for(int i=0; i<length(); i++)
	{
		CCouncilAction *Action = (CCouncilAction*)get(i);
		if ((Action->get_type() == CAction::ACTION_COUNCIL_DECLARE_WAR)
					&& (Action->get_argument() == mCouncilID))	
			return Action;
	}
	return NULL;
}

CCouncilAction*
CCouncilActionList::get_break_pact_action(int mCouncilID)
{
	for(int i=0; i<length(); i++)
	{
		CCouncilAction *Action = (CCouncilAction*)get(i);
		if ((Action->get_type() == CAction::ACTION_COUNCIL_BREAK_PACT)
				&& (Action->get_argument() == mCouncilID))	
			return Action;
	}
	return NULL;
}

CCouncilAction*
CCouncilActionList::get_break_ally_action(int mCouncilID)
{
	for(int i=0; i<length(); i++)
	{
		CCouncilAction *Action = (CCouncilAction*)get(i);
		if ((Action->get_type() == CAction::ACTION_COUNCIL_BREAK_ALLY)
				&& (Action->get_argument() == mCouncilID))	
			return Action;
	}
	return NULL;
}

CCouncilAction*
CCouncilActionList::get_improve_relation_action(int mCouncilID)
{
	for(int i=0; i<length(); i++)
	{
		CCouncilAction *Action = (CCouncilAction*)get(i);
		if ((Action->get_type() == 
						CAction::ACTION_COUNCIL_IMPROVE_RELATION)
				&& (Action->get_argument() == mCouncilID))	
			return Action;
	}
	return NULL;
}
