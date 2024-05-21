#include <libintl.h>
#include "../../pages.h"
#include"../../archspace.h"

bool
CPageExpedition::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	/*if ((!GAME->mUpdateTurn))
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Expeditions are disabled during prestart."));
		return output("error.html");
	}*/
	CFleetList *
		FleetList = aPlayer->get_fleet_list();

	if (FleetList->fleet_number_by_status(CFleet::FLEET_STAND_BY) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You don't have any fleets on stand-by."));
		return output("fleet/expedition_error.html");
	}
	if ((aPlayer->get_protected_mode() == CPlayer::PROTECTED_BEGINNER) && (aPlayer->get_planet_list()->length() == 3))
	{
		ITEM("STRING_WARNING",
				GETTEXT("The Imperial Fleets currently protecting your empire can only manage the protection of three planets. Should you continue to expand further they will be forced to withdraw pre-maturely."));
	}
	else
	{
		ITEM("STRING_WARNING",
				GETTEXT("&nbsp;"));
	}
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		CMission&
			Mission = Fleet->get_mission();

		if (Mission.get_mission() == CMission::MISSION_EXPEDITION ||
			Mission.get_mission() == CMission::MISSION_RETURNING_WITH_PLANET)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("There's a fleet on expedition or returning from expedition."));
					return output("fleet/expedition_error.html");
		}
	}

	ITEM("STRING_FLEET_STATUS", GETTEXT("Fleet Status"));

	ITEM("FLEETS", FleetList->expedition_fleet_list_html(aPlayer));

//	system_log("end page handler %s", get_name());

	return output( "fleet/expedition.html" );
}

