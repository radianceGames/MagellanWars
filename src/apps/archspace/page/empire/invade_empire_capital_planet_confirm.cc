#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../define.h"

bool
CPageEmpireInvadeEmpireCapitalPlanetConfirm::handler(CPlayer *aPlayer)
{
    
//	system_log( "start page handler %s", get_name() );
#ifdef PROTECTION_BEGINNER
	if (aPlayer->get_protected_mode() == CPlayer::PROTECTED_BEGINNER)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You cannot attack the empire for it is protecting you")));
		return output("empire/invade_empire_error.html");
	}
#endif

	int
		HomeClusterID = aPlayer->get_home_cluster_id();

	CMagistrateList *
		MagistrateList = EMPIRE->get_magistrate_list();
	CMagistrate *
		Magistrate = MagistrateList->get_by_cluster_id(HomeClusterID);

	if (Magistrate == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no magistrate in the cluster(#%1$s)."),
							dec2unit(HomeClusterID)));
		return output("empire/invade_empire_error.html");
	}
	if (Magistrate->is_dead() == false)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("The magistrate in the cluster(#%1$s) is still alive. So you can't attack The Empire's planets yet."),
							dec2unit(HomeClusterID)));
		return output("empire/invade_empire_error.html");
	}

	if (EMPIRE->get_number_of_empire_planets() > 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Not all the Empire' planets have taken by players yet. So you can't attack The Empire's fortress yet."));
		return output("empire/invade_empire_error.html");
	}

	CFortressList *
		FortressList = EMPIRE->get_fortress_list();
	CCouncil *
		Council = aPlayer->get_council();

	if (FortressList->get_number_of_fortresses_taken_by_council(Council, 4) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("To attack the Empire capital planet, your council must have at least 1 fortress in the 4th layer."));
		return output("empire/invade_empire_error.html");
	}

	if (CEmpire::mEmpireInvasionLimit != 0)
	{
		CPlayerActionList *
			PlayerActionList = aPlayer->get_action_list();

		int
			InvasionHistory = 0;
		time_t
			EffectiveStartTime = CGame::get_game_time() - CEmpire::mEmpireInvasionLimitDuration;
		for (int i=0 ; i<PlayerActionList->length() ; i++)
		{
			CPlayerAction *
				PlayerAction = (CPlayerAction *)PlayerActionList->get(0);
			if (PlayerAction->get_type() != CAction::ACTION_PLAYER_EMPIRE_INVASION_HISTORY) continue;
			if (PlayerAction->get_start_time() > EffectiveStartTime) InvasionHistory++;
		}

		if (InvasionHistory >= CEmpire::mEmpireInvasionLimit)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You are trying to attack the empire too often. Take a little rest."));
			return output("empire/invade_empire_error.html");
		}
	}

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	if (FleetList->fleet_number_by_status(CFleet::FLEET_STAND_BY) == 0)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You don't have any stand-by fleets."));
		return output("empire/invade_empire_error.html");
	}

	ITEM("CONFIRM_MESSAGE",
			GETTEXT("The road to the Imperial Capital of Caput Mundi lays open to your fleets."
				" The unequalled fame and power are within grasp upon victory, and you may even found your own empire.<BR><BR>\n"
				"But you will be faced with the best of the best. The Imperial Guard Fleets, strongest Elite Forces and powerful defense forts will resist to the last man, and it won't be an easy fight."
				" The resistance wouldn't be like anything you've seen before.<BR><BR>\n"
				"Will you launch the military campaign upon the Imperial Capital?"));

//	system_log( "end page handler %s", get_name() );

	return output("empire/invade_empire_capital_planet_confirm.html");
}

