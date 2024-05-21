#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../define.h"

bool
CPageEmpireInvadeEmpirePlanetFleet::handler(CPlayer *aPlayer)
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
				(char *)format(GETTEXT("The magistrate in the cluster(#%1$s) is still alive. You can't attack The Empire's planets yet."),
							dec2unit(HomeClusterID)));
		return output("empire/invade_empire_error.html");
	}

	if (EMPIRE->get_number_of_empire_planets() == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("All the Empire's planets have been taken by players already. Now you may attack the Empire's fortress."));
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
					GETTEXT("You are trying to attack the Empire too often. Take a little rest."));
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

	ITEM("STRING_THE_NUMBER_OF_LEFT_PLANETS_IN_THE_EMPIRE_CLUSTER",
			GETTEXT("The number of planets left in the Empire cluster"));
	ITEM("THE_NUMBER_OF_LEFT_PLANETS_IN_THE_EMPIRE_CLUSTER",
			dec2unit(EMPIRE->get_number_of_empire_planets()));

	ITEM("STRING_THE_NUMBER_OF_TAKEN_PLANETS_BY_YOU",
			GETTEXT("The number of planets taken by you"));
	ITEM("THE_NUMBER_OF_TAKEN_PLANETS_BY_YOU",
			dec2unit(EMPIRE_PLANET_INFO_LIST->get_number_of_planets_by_owner(aPlayer, CEmpire::LAYER_EMPIRE_PLANET)));

	ITEM("STRING_FLEET_DEPLOYMENT", GETTEXT("Fleet Deployment"));

	ITEM("TARGET_PLAYER",
			(char *)format(GETTEXT("Target Selected - %1$s"), 
					EMPIRE->get_nick()));

	ITEM("SELECT_FLEET_MESSAGE", 
			GETTEXT("Select fleets to deploy(up to 20 fleets)."));

	ITEM("FLEETS_TO_SEND", FleetList->siege_planet_fleet_list_html(aPlayer));
  	// if player doesn't have a preference object yet, make it
   	if (aPlayer->get_preference() == NULL)
      	aPlayer->set_preference(new CPreference(aPlayer->get_game_id()));	
	CPreference *
		aPreference = aPlayer->get_preference();
	ITEM("PREFERENCE_PREFIX", GETTEXT("Deploy fleets using:"));

	CString HTMLSelectOptions;
	HTMLSelectOptions.clear();
	for (int i=0; i <= CPreference::PR_JAVASCRIPT; i++)
    {
      if (aPreference->has(i)) 
          HTMLSelectOptions.format("<OPTION VALUE=%d SELECTED>%s</OPTION>\n",i,aPreference->EPreferencetoString((CPreference::EPreference)i)); 
      else
          HTMLSelectOptions.format("<OPTION VALUE=%d>%s</OPTION>\n",i,aPreference->EPreferencetoString((CPreference::EPreference)i));
    } 
	
	ITEM("SELECT_OPTIONS", (char*)HTMLSelectOptions);
        	

//	system_log( "end page handler %s", get_name() );

	return output("empire/invade_empire_planet_fleet.html");
}

