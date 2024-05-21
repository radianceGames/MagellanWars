#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"
bool
CPageMissionStationResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

        if (!CGame::mUpdateTurn)
        {
                ITEM("ERROR_MESSAGE", GETTEXT("You cannot perform this action until time starts."));
                return output("fleet/mission_error.html");
        }

	static CString
		Message;
	Message.clear();

	CCommandSet
		FleetSet;
	FleetSet.clear();

	QUERY("FLEET_SET", FleetSetString);
	if (!FleetSetString)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
		return output("fleet/mission_error.html");
	}

	FleetSet.set_string(FleetSetString);
	if (FleetSet.is_empty())
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select any fleets."));
		return output("fleet/mission_error.html");
	}

	QUERY("PLANET", PlanetIDString);
	int
		PlanetID = as_atoi(PlanetIDString);

	CPlanet *
		Planet = aPlayer->get_planet(PlanetID);
	if(!Planet)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You don't have such a planet."));
		return output("fleet/mission_error.html");
	}

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;
		if (FleetSet.has(i))
		{
			Fleet->init_mission(CMission::MISSION_STATION_ON_PLANET, PlanetID);
		}
	}

	Message.format(GETTEXT("Your fleet(s) has been dispatched to the planet %1$s."),
					Planet->get_name());
	Message += "<BR>\n";
	Message.format(GETTEXT("Your fleet(s) will detect and interpret enemy invasion,<BR>and dispel special operations and privateers on %1$s."),
					Planet->get_name());
	Message += "<BR><BR>\n";
	Message += GETTEXT("You may recall the fleet with the Recall menu any time you want to do.");

	ITEM("RESULT_MESSAGE", (char *)Message);

//	system_log("end page handler %s", get_name());

	return output("fleet/mission_station_result.html");
}

