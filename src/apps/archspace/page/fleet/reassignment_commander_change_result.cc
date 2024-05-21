#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageReassignmentCommanderChangeResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		Message;
	Message.clear();

	QUERY("FLEET", FleetIDString);

	int
		FleetID = as_atoi(FleetIDString);
	CFleet *
		Fleet = aPlayer->get_fleet_list()->get_by_id(FleetID);
	if (!Fleet)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You don't have such a fleet with ID %1$s."),
								dec2unit(FleetID)));
		return output("fleet/reassignment_error.html");
	}

	if(Fleet->get_status() != CFleet::FLEET_STAND_BY)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You can reassign only stand-by fleets.")));
		return output("fleet/reassignment_error.html");
	}

	QUERY("COMMANDER", NewCommanderIDString);

	int
		NewCommanderID = as_atoi(NewCommanderIDString);
	CAdmiral *
		NewCommander = aPlayer->get_admiral_pool()->get_by_id(NewCommanderID);
	if(!NewCommander)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format("You entered wrong admiral id %d.", NewCommanderID));
		return output("fleet/reassignment_error.html");
	}

	CAdmiral *
		Admiral = aPlayer->get_admiral_list()->get_by_id(Fleet->get_admiral_id());

	int
		LostExp = Fleet->get_exp()/5;	// 20%
	Fleet->change_exp(-LostExp);

	Admiral->set_fleet_number(0);
	NewCommander->set_fleet_number(Fleet->get_id());
	Fleet->set_admiral(NewCommander->get_id());

	aPlayer->get_admiral_list()->remove_without_free_admiral(Admiral->get_id());
	aPlayer->get_admiral_pool()->add_admiral(Admiral);
	aPlayer->get_admiral_pool()->remove_without_free_admiral(NewCommander->get_id());
	aPlayer->get_admiral_list()->add_admiral(NewCommander);

	Admiral->type(QUERY_UPDATE);
	STORE_CENTER->store(*Admiral);

	NewCommander->type(QUERY_UPDATE);
	STORE_CENTER->store(*NewCommander);

	Fleet->type(QUERY_UPDATE);
	STORE_CENTER->store(*Fleet);

	Message.format(GETTEXT("You have selected %1$s as a fleet commander for %2$s."),
					NewCommander->get_name(),
					Fleet->get_name());
	Message += "<BR>\n";

	Message.format(GETTEXT("%1$s has lost %2$s exp."),
					Fleet->get_name(),
					dec2unit(LostExp));
	Message += "<BR>\n";

	int
		DroppedShip = Fleet->get_current_ship() - NewCommander->get_fleet_commanding();

	if(DroppedShip > 0)
	{
		aPlayer->get_dock()->change_ship((CShipDesign*)Fleet, DroppedShip);
		Fleet->set_current_ship(NewCommander->get_fleet_commanding());
		Fleet->set_max_ship(NewCommander->get_fleet_commanding());
		Message.format("%d ship(s) were dropped from the fleet.<BR>\n", DroppedShip);
	}

	ITEM("RESULT_MESSAGE", (char *)Message);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/reassignment_commander_change_result.html");
}

