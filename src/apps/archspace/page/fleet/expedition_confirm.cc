#include <libintl.h>
#include "../../pages.h"
#include"../../archspace.h"
bool
	CPageExpeditionConfirm::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());
	/*if ((!GAME->mUpdateTurn))
	{
		ITEM("ERROR_MESSAGE",
			GETTEXT("Expeditions are disabled during prestart."));
		return output("error.html");
	}*/
	/*if((aPlayer->get_turn() < 200 && aPlayer->get_planet_list()->length() >= 3))
	{
		ITEM("ERROR_MESSAGE",
			GETTEXT("You cannot Expedition more then 2 planets before turn 200."));
		return output("error.html");
	}*/
	CFleetList *
	FleetList = aPlayer->get_fleet_list();

	QUERY("FLEET_ID", FleetIDString);
	int
		FleetID = as_atoi(FleetIDString);
	CFleet *
	Fleet = FleetList->get_by_id(FleetID);

	if (!Fleet)
	{
		ITEM("ERROR_MESSAGE",
			(char *)format(GETTEXT("You don't have any fleet with ID %s."),
				dec2unit(FleetID)));
		return output("fleet/expedition_error.html");
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

	static CString
		Message;
	Message.clear();

	Message.format("%s<BR>\n",
		GETTEXT("Fleets on a mission will not participate"
			" in the defense plan,<BR>"
			"and once they enter a mission,"
			" you cannot change status"
			" until the mission is finishes."));
	Message += "<BR>\n";
	Message += GETTEXT("Your selected fleet may find a new planet on its trip,<BR>"
			"or may encounter some strange events.");
	Message += "<BR><BR>\n";
	Message += GETTEXT("Are you sure you want to send the fleet on an expedition?");

	ITEM("CONFIRM_MESSAGE", (char *)Message);

	ITEM("FLEET_ID", FleetID);

//	system_log("end page handler %s", get_name());

	return output( "fleet/expedition_confirm.html" );
}
