#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageReassignmentShipReassignmentResult::handler(CPlayer *aPlayer)
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

	CAdmiral *
		Admiral = aPlayer->get_admiral_list()->get_by_id(Fleet->get_admiral_id());

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

	CString
		ClassInfoURL;
	ClassInfoURL.clear();
	ClassInfoURL.format("<A HREF=\"archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
						ShipDesign->get_design_id(),
						ShipDesign->get_name());

	CDock *
		Pool = aPlayer->get_dock();
	if (Pool->count_ship(ShipDesign->get_design_id()) < FleetNewSize)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You don't have %1$s %2$s ships."),
								dec2unit(FleetNewSize),
								(char *)ClassInfoURL));
		return output("fleet/reassignment_error.html");
	}
	
	
	
	
	
	int
		ChangeFleetSize = FleetNewSize - Fleet->get_current_ship();
	int LostExp = 0;
	int ShipExpDif = FleetNewSize - Fleet->get_max_ship(); 
	if (ShipExpDif > 0)
 	{
 		int BaseExp = 25 + aPlayer->get_control_model()->get_military()*3; //the min exp the fleets should ever have
 		int OriginalExpWeight = Fleet->get_max_ship()*Fleet->get_exp();
 		int NewExpWeight = ShipExpDif*BaseExp;
 		int LostExp = Fleet->get_exp() - (int)((OriginalExpWeight+NewExpWeight)/FleetNewSize);
 	}
	
	LostExp += (int)(Fleet->get_exp() * .1);
	
	Fleet->change_exp(-LostExp);
	
	aPlayer->get_dock()->change_ship(ShipDesign, -FleetNewSize);
	aPlayer->get_dock()->change_ship((CShipDesign *)Fleet, Fleet->get_current_ship());

	Fleet->set_ship_class(ShipDesign);
	Fleet->set_max_ship(FleetNewSize);
	Fleet->set_current_ship(FleetNewSize);

	aPlayer->get_fleet_list()->refresh_power();	//telecard 2001/06/13

	Fleet->type(QUERY_UPDATE);
	STORE_CENTER->store(*Fleet);

	Message.format(GETTEXT("You have successfully reassigned %1$s with %2$s of %3$s ships."),
					Fleet->get_name(),
					dec2unit(FleetNewSize),
					(char *)ClassInfoURL);
	Message += "<BR>\n";
	Message.format("%s has lost %s exp.",
					Fleet->get_name(), dec2unit(LostExp));

	ITEM("RESULT_MESSAGE", (char *)Message);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/reassignment_ship_reassignment_result.html");
}

