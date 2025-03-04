#include "../../pages.h"
#include "../../player.h"
#include "../../archspace.h"

bool
CPageEmpireBounty::handler(CPlayer *aPlayer)
{
	// Messy Code for pretty dynamic output
	OFFERED_BOUNTY_TABLE->refresh_bounties();
	ITEM( "DESCRIPTION", (char*)format("Here you can accept bounties to improve your relation with the empire."
					"<br> You can only pursue %d bounties at a time."
					"<br> If you fail to gain 5 empire points on a bounty within %s "
					"the empire will be displeased.",MAX_PLAYER_BOUNTIES,timetostring((time_t)BOUNTY_DURATION)));
	ITEM( "EMPIRE_RELATION", aPlayer->get_degree_name_of_empire_relation() );
	ITEM( "COURT_RANK", aPlayer->get_court_rank_name() );
	char *response = (char*)aPlayer->current_bounties_html();
	if (response)
	{
		ITEM( "CURRENT_BOUNTIES", response);
	} 
	else
	{
		ITEM( "CURRENT_BOUNTIES", "You are currently pursuing no bounties");
	}
	if (aPlayer->active_bounties() >= MAX_PLAYER_BOUNTIES)
	{
		ITEM( "AVAILABLE_BOUNTIES", "You already have as many bounties as you possibly can.");
	}
	else
	{
		response = (char*)aPlayer->possible_bounties_html();
		if (response) 
		{
			ITEM( "AVAILABLE_BOUNTIES", response);
		}
		else
		{
			ITEM( "AVAILABLE_BOUNTIES", "There are no bounties available for you");
		}
	}
	return output("empire/bounty.html");
}
