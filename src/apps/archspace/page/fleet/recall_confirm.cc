#include <libintl.h>
#include "../../pages.h"

bool
CPageRecallConfirm::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	static CString
		Message;
	Message.clear();

	CCommandSet
		FleetSet;
	FleetSet.clear();

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);

		CMission &
			Mission = Fleet->get_mission();
		if (Mission.get_mission() != CMission::MISSION_STATION_ON_PLANET &&
			Mission.get_mission() != CMission::MISSION_DISPATCH_TO_ALLY &&
			Mission.get_mission() != CMission::MISSION_EXPEDITION &&
			Mission.get_mission() != CMission::MISSION_TRAIN) continue;

		CString
			QueryVar;
		QueryVar.clear();
		QueryVar.format("FLEET%d", i);

		QUERY(QueryVar, FleetIDString);

		if (FleetIDString)
		{
			if (!strcasecmp(FleetIDString, "ON"))
			{
				FleetSet += i;
			}
		}
	}

	if (FleetSet.is_empty())
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You selected no fleet(s)."));
		return output("fleet/recall_error.html");
	}

	Message = GETTEXT("You have selected the following fleet(s) to return.");
	Message += "<BR>\n";

	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		if (FleetSet.has(i))
		{
			Message += Fleet->get_name();
			Message += " ";
			Message += Fleet->get_detailed_status_string();
			Message += "<BR>\n";
		}
	}
	Message += "<BR>\n";
	Message += GETTEXT("Are you sure you want to recall the fleets?");

	ITEM("CONFIRM_MESSAGE", (char *)Message);

	ITEM("FLEET_SET",
			(char *)format("<INPUT TYPE=hidden NAME=FLEET_SET VALUE=%s>\n",
							FleetSet.get_string()));

//	system_log("end page handler %s", get_name());

	return output("fleet/recall_confirm.html");
}

