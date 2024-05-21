#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"
#include "../../bounty.h"
#include "../../archspace.h"
#include <cmath>

bool
CPageEmpireBountyResult::handler(CPlayer *aPlayer)
{
	QUERY("TARGET", TargetPlayerStr);

	if (!CGame::mUpdateTurn)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("The empire will not accept bribes until time has started."));
		return output("empire/bounty_result.html");
	}    

	if (!TargetPlayerStr)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("You did not select a target player."));
		return output("empire/bounty_result.html");
	}
	
	if (aPlayer->active_bounties() >= MAX_PLAYER_BOUNTIES)
	{
		ITEM( "RESULT_MESSAGE", "You already have as many bounties as you possibly can.");
		return output("empire/bounty_result.html");
	}

	int
		PlayerId = as_atoi(TargetPlayerStr);
	COfferedBounty *aBounty = OFFERED_BOUNTY_TABLE->get_by_id(PlayerId);
	if (!aBounty)
	{
		ITEM("RESULT_MESSAGE", 
				GETTEXT("A bounty was not found for that player."));
		return output("empire/bounty_result.html");
	}
	
	if (aBounty->get_player()->get_game_id() == aPlayer->get_game_id())
	{
		ITEM("RESULT_MESSAGE", 
				GETTEXT("You cannot accept a bounty on yourself"));
		return output("empire/bounty_confirm.html");
	}
	
	if (aPlayer->get_bounty_by_player_id(aBounty->get_player()->get_game_id()) != NULL)
	{
		ITEM("RESULT_MESSAGE", 
				GETTEXT("You already accepted a bounty on this player"));
		return output("empire/bounty_confirm.html");
	}
	
	int hunterPowerMin, hunterPowerMax, currentPower;
	
	hunterPowerMin = (int)(aPlayer->get_power()/2);
	hunterPowerMax = (int)(aPlayer->get_power()*2);
	currentPower = aBounty->get_player()->get_power();
	if (!((currentPower > hunterPowerMin) && (currentPower < hunterPowerMax)))
	{
		ITEM("RESULT_MESSAGE", 
				GETTEXT("The bounty is out of your attack range"));
		return output("empire/bounty_result.html");
	}
	
	aBounty->add_hunter(aPlayer);

	ITEM("RESULT_MESSAGE", 
			GETTEXT("You have accepted the bounty."));

	return output("empire/bounty_result.html");
}
