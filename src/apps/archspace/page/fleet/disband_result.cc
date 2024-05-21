#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageDisbandResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		FleetName;
	FleetName.clear();

	static CString
		AdmiralName;
	AdmiralName.clear();

	CCommandSet
		FleetSet;
	FleetSet.clear();

	QUERY("FLEET_SET", FleetSetString);
	if (!FleetSetString)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("No fleet data have been entered."));
		return output("fleet/disband_error.html");
	}
	FleetSet.set_string(FleetSetString);

	if (FleetSet.is_empty())
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You selected no fleet(s)."));
		return output("fleet/disband_error.html");
	}

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	CDock *
		Dock = aPlayer->get_dock();
	CAdmiralList *
		AdmiralList = aPlayer->get_admiral_list();
	CAdmiralList *
		AdmiralPool = aPlayer->get_admiral_pool();

	bool
		Comma = false;
	int
		CurrentFleetNumber = FleetList->length();

	for (int i=CurrentFleetNumber-1 ; i>=0 ; i--)
	{
		CFleet *
			Fleet = (CFleet *)FleetList->get(i);
		CAdmiral *
			Admiral = (CAdmiral *)AdmiralList->get_by_id(Fleet->get_admiral_id());

		if (!Admiral)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(
							GETTEXT("Fleet %1$s's admiral does not exist."),
							Fleet->get_name()));
			return output("fleet/disband_error.html");
		}
		if (!FleetSet.has(i)) continue;
		if (Fleet->get_status() != CFleet::FLEET_STAND_BY) continue;

		if (Comma)
		{
			FleetName += ", ";
			AdmiralName += ", ";
		}
		FleetName += Fleet->get_nick();
		AdmiralName += Admiral->get_nick();

		Fleet->type(QUERY_DELETE);
		*STORE_CENTER << *Fleet;

		Dock->change_ship((CShipDesign *)Fleet, Fleet->get_current_ship());
		FleetList->remove_fleet(Fleet->get_id());

		Admiral->set_fleet_number(0);
		AdmiralList->remove_without_free_admiral(Admiral->get_id());
		AdmiralPool->add_admiral(Admiral);

		Admiral->type(QUERY_UPDATE);
		*STORE_CENTER << *Admiral;
		//start telecard 2001/01/22
		Comma = true;
		//end telecard
	}

	if (Comma == false)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The fleet(s) that you have chosen doesn't/don't exist."));
	}
	else
	{
		ITEM("RESULT_MESSAGE",
				(char *)format(GETTEXT("You have successfully disbanded the fleet(s) %1$s. And the commander(s) %2$s returned to the commander pool."),
								(char *)FleetName,
								(char *)AdmiralName));
	}

//	system_log( "end page handler %s", get_name() );

	return output("fleet/disband_result.html");
}

