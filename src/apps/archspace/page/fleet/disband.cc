#include <libintl.h>
#include "../../pages.h"

bool
CPageDisband::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	ITEM("STRING_FLEET_STATUS", GETTEXT("Fleet Status"));

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	if (!FleetList)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("FleetList is NULL. Ask Archspace Development Team."));
		return output("fleet/disband_error.html");
	}

	bool
		AnyStandbyFleet = false;
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue; 

		AnyStandbyFleet = true;
		break;
	}

	if (!AnyStandbyFleet)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Currently there are no fleets on stand-by."));
		return output("fleet/disband_error.html");
	}

	ITEM ("FLEETS", FleetList->disband_fleet_list_html(aPlayer));

//	system_log("end page handler %s", get_name());

	return output( "fleet/disband.html" );
}

