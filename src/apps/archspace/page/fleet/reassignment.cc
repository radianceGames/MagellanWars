#include <libintl.h>
#include "../../pages.h"

bool
CPageReassignment::handler(CPlayer *aPlayer)
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
		return output("fleet/reassignment_error.html");
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
					(char *)format(GETTEXT("There is no admiral with ID %1$s in you admiral list. Please ask Archspace Development Team."),
									dec2unit(Fleet->get_admiral_id())));
			return output("fleet/reassignment_error.html");
		}
	}

	ITEM("STRING_FLEET_STATUS", GETTEXT("Fleet Status"));

	bool
		AnyStandbyFleet = false;
	for(int i=0 ; i<FleetList->length() ; i++)
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
				GETTEXT("Currently there is no fleet on stand-by status."));
		return output("fleet/recall_error.html");
	}

	ITEM("JS_FLEET_COMMANDER_NUMBER", AdmiralList->length());

    ITEM("REASSIGNMENT_JAVASCRIPT", GETTEXT(" "));
	//ITEM("REASSIGNMENT_JAVASCRIPT", FleetList->reassignment_fleet_list_javascript(aPlayer));

    ITEM("REASSIGNMENT_FLEETS", FleetList->reassignment_fleet_list_html(aPlayer));
	//ITEM("REASSIGNMENT_FLEETS", FleetList->reassignment_fleet_list_html());

	ITEM("SELECT_REASSIGNMENT", CFleet::select_reassignment_html());

//	system_log("end page handler %s", get_name());

	return output( "fleet/reassignment.html" );
}

