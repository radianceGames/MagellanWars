#include "../triggers.h"
#include "../archspace.h"

bool
CTriggerAction::handler()
{
	if (!CGame::mUpdateTurn) return true;
	if (EMPIRE->is_dead() == true) return true;

	PLAYER_ACTION_TABLE->expire();
	COUNCIL_ACTION_TABLE->expire();

	return true;
}
