#include "../triggers.h"
#include "../archspace.h"

bool
CTriggerRelationTimeout::handler()
{
   	if (!CGame::mUpdateTurn) return true;
	if (EMPIRE->is_dead() == true) return true;

	PLAYER_RELATION_TABLE->timeout();
	COUNCIL_RELATION_TABLE->timeout();

//	SLOG("check action expire");
	return true;
}
