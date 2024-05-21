#include <libintl.h>
#include "../../pages.h"

bool
CPageMission::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	CAdmiralList *
		AdmiralList = aPlayer->get_admiral_list();

	if (FleetList->fleet_number_by_status(CFleet::FLEET_STAND_BY) == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You don't have any fleets on stand-by."));
		return output("fleet/mission_error.html");
	}

	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		CAdmiral *
			Admiral = AdmiralList->get_by_id(Fleet->get_admiral_id());

		if (!Admiral)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no admiral with ID %1$s in your admiral list. Please ask Archspace Development team."),
									dec2unit(Fleet->get_admiral_id())));
			return output("fleet/expedition_error.html");
		}
	}

	ITEM("STRING_FLEET_STATUS", GETTEXT("Fleet Status"));

	ITEM("FLEETS", FleetList->mission_fleet_list_html(aPlayer));

	ITEM("SELECT_MISSION", CMission::select_mission_html());

//	system_log("end page handler %s", get_name());

	return output( "fleet/mission.html" );
}

