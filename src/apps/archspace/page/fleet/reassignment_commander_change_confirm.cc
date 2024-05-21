#include <libintl.h>
#include "../../pages.h"

bool
CPageReassignmentCommanderChangeConfirm::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		Message;
	Message.clear();

	ITEM("STRING_COMMANDER_CHANGE", GETTEXT("Commander Change"));

	QUERY("FLEET", FleetIDString);
	int
		FleetID;
	FleetID = as_atoi(FleetIDString);

	QUERY("FREE_ADMIRAL", NewCommanderIDString);
	int
		NewCommanderID;
	NewCommanderID = as_atoi(NewCommanderIDString);

	CFleet
		*Fleet = aPlayer->get_fleet_list()->get_by_id(FleetID);
	if(!Fleet)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You don't have such a fleet with ID %1$s."),
								dec2unit(FleetID)));
		return output("fleet/reassignment_commander_error.html");
	}

	if (Fleet->get_status() != CFleet::FLEET_STAND_BY)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You can reassign only stand-by fleets."));
		return output("fleet/reassignment_commander_error.html");
	}

	CAdmiral
		*NewCommander = aPlayer->get_admiral_pool()->get_by_id(NewCommanderID);
	if(!NewCommander)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format("You entered wrong admiral id %d.", NewCommanderID));
		return output("fleet/reassignment_commander_error.html");
	}

	CAdmiral
		*Admiral = aPlayer->get_admiral_list()->get_by_id(Fleet->get_admiral_id());

	ITEM("REASSIGNMENT_FLEET",
			(char *)format("<INPUT TYPE=hidden NAME=FLEET VALUE=%d>\n",
							Fleet->get_id()));

	ITEM("REASSIGNMENT_COMMANDER",
			(char *)format("<INPUT TYPE=hidden NAME=COMMANDER VALUE=%d>\n",
							NewCommander->get_id()));

	CString
		CommanderInfoURL;
	CommanderInfoURL.clear();

	CommanderInfoURL.format("<A HREF=\"/archspace/fleet/fleet_commander_information.as?ADMIRAL_ID=%d\">%s</A>",
							NewCommander->get_id(),
							NewCommander->get_name());

	Message.format(GETTEXT("If you select %1$s as the fleet commander, you will lose 20%% of %2$s(#%3$d)'s exp."),
					(char *)CommanderInfoURL,
					Fleet->get_name(),
					Fleet->get_id());

	if( (NewCommander->get_fleet_commanding() < Admiral->get_fleet_commanding()) &&
		(NewCommander->get_fleet_commanding() < Fleet->get_current_ship()) )
	{
		int ShipDropped = Fleet->get_current_ship() - NewCommander->get_fleet_commanding();
		Message.format(GETTEXT("The new commander can control only %1$s ships, but %2$s ships are in this fleet now.<BR>"
								"So %3$s ships will be dropped from the fleet."),
						dec2unit(NewCommander->get_fleet_commanding()),
						dec2unit(Fleet->get_current_ship()),
						dec2unit(ShipDropped));
		Message += "<BR>\n";
	}

	ITEM("CONFIRM_MESSAGE", (char *)Message);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/reassignment_commander_change_confirm.html");
}

