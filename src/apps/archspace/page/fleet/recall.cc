#include <libintl.h>
#include "../../pages.h"

bool
CPageRecall::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	ITEM("STRING_DISPATCHED_FLEET_S_", GETTEXT("Dispatched Fleet(s)"));

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	if (!FleetList)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Fleet List is NULL."
						" Please ask Archspace Development Team."));
		return output("fleet/recall_error.html");
	}

	CAdmiralList *
		AdmiralList = aPlayer->get_admiral_list();
	if (!AdmiralList)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Admiral List is NULL."
						" Please ask Archspace Development Team."));
		return output("fleet/recall_error.html");
	}

	bool
		AnyRecallFleet = false;
	for(int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);

		CMission &
			Mission = Fleet->get_mission();
		if (Mission.get_mission() != CMission::MISSION_STATION_ON_PLANET &&
			Mission.get_mission() != CMission::MISSION_DISPATCH_TO_ALLY &&
			Mission.get_mission() != CMission::MISSION_EXPEDITION &&
			Mission.get_mission() != CMission::MISSION_TRAIN) continue;

		AnyRecallFleet = true;
		break;
	}

	if (!AnyRecallFleet)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Currently there are no activated fleets under mission."));
		return output("fleet/recall_error.html");
	}

	ITEM ("DISPATCHED_FLEET_LIST", FleetList->recall_fleet_list_html(aPlayer));

//	system_log("end page handler %s", get_name());

	return output( "fleet/recall.html" );
}

