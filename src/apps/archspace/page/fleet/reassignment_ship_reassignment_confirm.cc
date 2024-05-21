#include <libintl.h>
#include "../../pages.h"

bool
CPageReassignmentShipReassignmentConfirm::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		Message;
	Message.clear();

	ITEM("STRING_SHIP_REASSIGNMENT", GETTEXT("Ship Reassignment"));

	QUERY("FLEET", FleetIDString);
	int
		FleetID = as_atoi(FleetIDString);
	CFleet *
		Fleet = aPlayer->get_fleet_list()->get_by_id(FleetID);
	if(!Fleet)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You don't have such a fleet with ID %1$s."),
								dec2unit(FleetID)));
		return output("fleet/reassignment_error.html");
	}
	if(Fleet->get_status() != CFleet::FLEET_STAND_BY)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You can reassign only stand-by fleets."));
		return output("fleet/reassignment_error.html");
	}

	QUERY("FLEET_NEW_SIZE", FleetNewSizeString);
	int
		FleetNewSize = as_atoi(FleetNewSizeString);

	if(FleetNewSize <= 0)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You entered invalid value(%1$s) for the new number of ships."),
								dec2unit(FleetNewSize)));
		return output("fleet/reassignment_error.html");
	}

	QUERY("SHIP", ShipIDString);
	int
		ShipID = as_atoi(ShipIDString);
	CShipDesign *
		ShipDesign = aPlayer->get_ship_design_list()->get_by_id(ShipID);
	if(!ShipDesign)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(
						GETTEXT("You don't have such a designed class with ID %1$s."),
						dec2unit(ShipID)));
		return output("fleet/reassignment_error.html");
	}
	// start telecard 2001/01/23
	if( ShipDesign->get_design_id() == Fleet->get_ship_class() )
	{	
		ITEM("ERROR_MESSAGE",
				GETTEXT("Use \"Remove/Add Ships\" instead of \"Ship Reassignment\" for just change number of ships.") );
		return output("fleet/reassignment_error.html");
	}
	// end telecard
	ITEM("REASSIGNMENT_FLEET",
			(char *)format("<INPUT TYPE=hidden NAME=FLEET VALUE=%d>\n",
							Fleet->get_id()));
	ITEM("FLEET_NEW_SIZE",
			(char *)format("<INPUT TYPE=hidden NAME=FLEET_NEW_SIZE VALUE=%d>\n",
							FleetNewSize));
	ITEM("REASSIGNMENT_SHIP",
			(char *)format("<INPUT TYPE=hidden NAME=SHIP VALUE=%d>\n", ShipID));

	CAdmiral *
		Admiral = aPlayer->get_admiral_list()->get_by_id(Fleet->get_admiral_id());

	if(!Admiral)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(
						GETTEXT("Fleet %1$s's admiral does not exist."),
						Fleet->get_name()));
		return output("fleet/reassignment_error.html");
	}
	if(FleetNewSize > Admiral->get_fleet_commanding())
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(
						GETTEXT("Fleet Commander %1$s can command %2$d ship(s)"
								" at most."),
						Admiral->get_name(),
						Admiral->get_fleet_commanding()));
		return output("fleet/reassignment_error.html");
	}

	CDock *
		Pool = (CDock *)aPlayer->get_dock();
	if(Pool->count_ship(ShipDesign->get_design_id()) < FleetNewSize)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You don't have %1$s %2$s ships."),
								dec2unit(FleetNewSize),
								ShipDesign->get_name()));
		return output("fleet/reassignment_error.html");
	}

	Message.format(GETTEXT("You have selected %1$s ship(s) of %2$s."),
					dec2unit(FleetNewSize),
					ShipDesign->get_name());
	Message.format(GETTEXT("Upgrading %1$s(#%2$d) will result in loss of 20%% of the fleet's exp."),
					Fleet->get_name(), Fleet->get_id());
	Message += GETTEXT("Are you sure you want to make the change?");
	Message += "<BR>\n";

	ITEM("CONFIRM_MESSAGE", (char *)Message);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/reassignment_ship_reassignment_confirm.html");
}

