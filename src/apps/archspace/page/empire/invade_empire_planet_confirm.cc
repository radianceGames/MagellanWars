#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../define.h"

bool
CPageEmpireInvadeEmpirePlanetConfirm::handler(CPlayer *aPlayer)
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

	CMagistrateList *
		MagistrateList = EMPIRE->get_magistrate_list();

	int
		ClusterID = aPlayer->get_home_cluster_id();
	CMagistrate *
		Magistrate = MagistrateList->get_by_cluster_id(ClusterID);

	if (Magistrate == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no magistrate in the cluster with ID %1$s."),
							dec2unit(ClusterID)));
		return output("empire/invade_empire_error.html");
	}
	if (Magistrate->is_dead() == false)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("The magistrate in the cluster(#%1$s) is still alive. So you can't attack The Empire's planets yet."),
							dec2unit(ClusterID)));
		return output("empire/invade_empire_error.html");
	}

	if (EMPIRE->get_number_of_empire_planets() == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("All the Empire' planets have taken by players already. Now you may attack the Empire's fortress."));
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
			GETTEXT("Please confirm that last order!!!<BR>\n"
					"You have begun to process an Attack against the Empire.<BR>\n"
					"If you do this, you will lose great favor/be forever a criminal!!!<BR>\n"
					"Do you really want to attack?"));

//	system_log( "end page handler %s", get_name() );

	return output("empire/invade_empire_planet_confirm.html");
}

