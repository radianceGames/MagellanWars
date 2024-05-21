#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"
#include "../../archspace.h"

bool
CPagePlayerDismissResult::handler(CPlayer *aPlayer)
{

	ARE_YOU_SPEAKER(aPlayer);

	CCouncil *myCouncil = aPlayer->get_council();

	ITEM("MENU_TITLE", GETTEXT("Player Dismissal"));
	
	QUERY("PLAYER_ID", PlayerIDString);
	if (!PlayerIDString) {
		ITEM("RESULT", GETTEXT("Invalid Player ID"));
		return output("council/player_dismiss_result.html");
	}
	int PlayerID = as_atoi(PlayerIDString);
		
	CPlayer *
		PlayerTarget = myCouncil->get_member_by_game_id(PlayerID);
	if (!PlayerTarget) {
		ITEM("RESULT", GETTEXT("Player does not belong to your council"));
		return output("council/player_dismiss_result.html");
	}
	
	if (!PlayerTarget->is_dismissable()) {
		ITEM("RESULT", GETTEXT("Player is not dismissable"));
		return output("council/player_dismiss_result.html");
	}
	
	//Remove player from council
	myCouncil->remove_member(PlayerID);
	
	//Find a new council
	CCouncil *Council;
	
	if (COUNCIL_TABLE->length() < CGame::mMinCouncilCount)
	{
		Council = GAME->create_new_council((CCluster*)UNIVERSE->get_by_id(PlayerTarget->get_home_cluster_id()));
	}
	else
	{
		int Available = 0;
		int Weight = 0;
		int Dice = 0;
		int Range = 0;
		for (int i=0 ; i<COUNCIL_TABLE->length() ; i++)
		{
			Council = (CCouncil *)COUNCIL_TABLE->get(i);
			if (Council->get_auto_assign() == false) continue;
			if (Council->get_id() == myCouncil->get_id()) continue;
			if (Council->get_number_of_members() > (int)(MAX_COUNCIL_MEMBER*0.8)) continue;
			if (Council->get_number_of_members() >= Council->max_member()) continue;
			if (Council->has_cluster() && !Council->has_cluster(PlayerTarget->get_home_cluster_id())) continue;
			Available++;
			Weight = MAX_COUNCIL_MEMBER*2 - Council->get_number_of_members();
			Range += Weight;
		}
		if (Available < CGame::mMinCouncilCount)
		{
			Council = GAME->create_new_council((CCluster*)UNIVERSE->get_by_id(PlayerTarget->get_home_cluster_id()));
		}
		else
		{
			Dice = number(Range)-1;
			Range = 0;
			for (int i=0 ; i<COUNCIL_TABLE->length() ; i++)
			{
				Council = (CCouncil *)COUNCIL_TABLE->get(i);
				if (Council->get_id() == myCouncil->get_id()) continue;
				if (Council->get_auto_assign() == false) continue;
				if (Council->get_number_of_members() > (int)(MAX_COUNCIL_MEMBER*0.8)) continue;
				if (Council->get_number_of_members() >= Council->max_member()) continue;
				if (Council->has_cluster() && !Council->has_cluster(PlayerTarget->get_home_cluster_id())) continue;
				Weight = MAX_COUNCIL_MEMBER*2 - Council->get_number_of_members();
				if (Dice >= Range && Dice < Range+Weight) break;
				Range += Weight;
			}
		}
	}
	Council->add_member(PlayerTarget);
	PlayerTarget->set_council(Council);
	COUNCIL_TABLE->add_council_rank(Council);
	COUNCIL_TABLE->refresh_rank_table();
	
	ITEM("RESULT", (char *)format(GETTEXT("Player: %s has been dismissed from your council"), PlayerTarget->get_nick()));
	return output("council/player_dismiss_result.html");
}
