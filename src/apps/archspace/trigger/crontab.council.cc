#include "../triggers.h"
#include "../define.h"
#include "../archspace.h"
#include "../council.h"

void
CCronTabCouncil::handler()
{
   	if (!CGame::mUpdateTurn) return;
	SLOG("SYSTEM : Council CronTab handler() start");

	if (EMPIRE->is_dead() == false)
	{
		for(int i=0; i<COUNCIL_TABLE->length(); i++)
		{
			CCouncil *
				Council = (CCouncil *)COUNCIL_TABLE->get(i);

			Council->update_by_time();
		}
#define	EIGHT_HOURS 28800
		GAME->mLastCouncilElection += EIGHT_HOURS;
#undef EIGHT_HOURS
	}

	GAME->mTechRate = 1;

	/*if (GAME->get_game_time() > 2*ONE_WEEK)
	{
		GAME->mTechRate = 3;
	}
	else if (GAME->get_game_time() > ONE_WEEK)
	{
		GAME->mTechRate = 2;
	}
	else if (PLAYER_TABLE->length() > 0)
	{
		float aTechRate = (PLAYER_TABLE->get_tech_rank_table()->get_top_number_of_techs()-20)/48;
		if (aTechRate < 1)
		{
			aTechRate = 1;
		}
		else if (aTechRate > 2)
		{
			aTechRate = 2;
		}
		GAME->mTechRate = (int)aTechRate;
	}
	else
	{
		GAME->mTechRate = 1;
	}*/

	SLOG("SYSTEM : Council CronTab handler() end");
}
