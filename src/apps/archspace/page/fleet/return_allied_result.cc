#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageRecallResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		Message;
	Message.clear();

	CCommandSet
		FleetSet;
	FleetSet.clear();

	CCommandSet FleetSet;
    FleetSet.clear();

    CFleetList *FleetList = aPlayer->get_fleet_list();
    for (int i=0 ; i<FleetList->length() ; i++) {
        CFleet *Fleet = (CFleet *)FleetList->get(i);
        if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

        CString QueryVar;
        QueryVar.clear();
        QueryVar.format("FLEET%d", i);

        QUERY(QueryVar, FleetIDString);

        if (FleetIDString) {
            if (!strcasecmp(FleetIDString, "ON")) {
                 FleetSet += i;
            }
        }
    }
	FleetSet.set_string(FleetSetString);
	if (FleetSet.is_empty())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You selected no fleet(s)."));
		return output("fleet/recall_error.html");
	}

	Message += GETTEXT("You have successfully recalled the following fleet(s).");
	Message += "<BR><BR>\n";

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		if (!FleetSet.has(i)) continue;

		CMission &Mission = Fleet->get_mission();
		if (Mission.get_mission() != CMission::MISSION_STATION_ON_PLANET &&
			Mission.get_mission() != CMission::MISSION_DISPATCH_TO_ALLY &&
			Mission.get_mission() != CMission::MISSION_EXPEDITION &&
			Mission.get_mission() != CMission::MISSION_TRAIN)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You can recall only fleets stationed on planets, on an expedition, training, or dispatched to allies."));
			return output("fleet/recall_error.html");
		}

		Message.format("%s %s<BR>\n",
						Fleet->get_name(), Fleet->get_detailed_status_string());
		Fleet->end_mission();

		Fleet->type(QUERY_UPDATE);
		STORE_CENTER->store(*Fleet);
	}

	ITEM("RESULT_MESSAGE", (char *)Message);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/recall_result.html");
}

