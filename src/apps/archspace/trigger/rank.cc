#include "../triggers.h"
#include "../archspace.h"
#include "../council.h"

bool
CTriggerRank::handler()
{
	system_log("trigger rank start");

	if (EMPIRE->is_dead() == false)
	{
		try
		{
		PLAYER_TABLE->refresh_rank_table();
		COUNCIL_TABLE->refresh_rank_table();
		}
		catch(...)
		{
			system_log("blob of rank Crapyness");
			handler();
			return true;
		}
	}

	system_log("trigger ranking end");

	return true;
}
