#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../player.h"
#include "../../preference.h"

bool
CPageDefensePlanGenericNew::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

    	// if player doesn't have a preference object yet, make it
    	if (aPlayer->get_preference() == NULL)
        	aPlayer->set_preference(new CPreference(aPlayer->get_game_id()));

	CPreference *
		aPreference = aPlayer->get_preference();

	CFleetList *
		FleetList = aPlayer->get_fleet_list();

	int
		ValidFleets = 0;
	for (int i = 0; i < FleetList->length(); i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		CMission &
				Mission = Fleet->get_mission();
		if ((Fleet->get_status() == CFleet::FLEET_STAND_BY) || (Mission.get_mission() == CMission::MISSION_RETURNING))
		{
			ValidFleets++;
		}
	}
	if (ValidFleets <= 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You do not have any fleets on stand-by.<BR>"
						"Please try again later."));
		return output("war/defense_plan_generic_new_error.html");
	}

	ITEM("PREFERENCE_PREFIX", GETTEXT("Deploy fleets using:"));

	ITEM("SELECT_OPTIONS", (char*)aPreference->HTMLSelectOptions());

	ITEM("SELECT_FLEET_MESSAGE",
			GETTEXT("Select fleets to deploy(up to 20 fleets)."));

	ITEM("FLEET_LIST", FleetList->deployment_fleet_list_html(aPlayer));

//	system_log( "end page handler %s", get_name() );

	return output( "war/defense_plan_generic_new.html" );
}

