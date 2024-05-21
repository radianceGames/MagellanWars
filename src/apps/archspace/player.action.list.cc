#include "action.h"

CPlayerActionList::~CPlayerActionList()
{
	remove_all();
}

bool 
CPlayerActionList::add_action(CPlayerAction* aAction)
{
	assert(aAction);

	if (find_sorted_key((TConstSomething)aAction->get_id()) >= 0) 
		return false;
	
	insert_sorted(aAction);

	return true;
}

CPlayerAction* 
CPlayerActionList::get_council_donation_action()
{
	for(int i=0; i<length(); i++)
	{
		CPlayerAction *Action = (CPlayerAction*)get(i);
		if (Action->get_type() == CAction::ACTION_PLAYER_COUNCIL_DONATION)
			return Action;
	}
	return NULL;
}

CPlayerAction*
CPlayerActionList::get_break_pact_action(int mPlayerID)
{
	for(int i=0; i<length(); i++)
	{
		CPlayerAction *Action = (CPlayerAction*)get(i);
		if ((Action->get_type() == CAction::ACTION_PLAYER_BREAK_PACT)
				&& (Action->get_argument() == mPlayerID))	
			return Action;
	}
	return NULL;
}

CPlayerAction*
CPlayerActionList::get_break_ally_action(int mPlayerID)
{
	for(int i=0; i<length(); i++)
	{
		CPlayerAction *Action = (CPlayerAction*)get(i);
		if ((Action->get_type() == CAction::ACTION_PLAYER_BREAK_ALLY)
				&& (Action->get_argument() == mPlayerID))	
			return Action;
	}
	return NULL;
}

CPlayerAction*
CPlayerActionList::get_declare_hostile_action(int mPlayerID)
{
	for(int i=0; i<length(); i++)
	{
		CPlayerAction *Action = (CPlayerAction*)get(i);
		if ((Action->get_type() == CAction::ACTION_PLAYER_DECLARE_HOSTILE)
				&& (Action->get_argument() == mPlayerID))	
			return Action;
	}
	return NULL;
}
