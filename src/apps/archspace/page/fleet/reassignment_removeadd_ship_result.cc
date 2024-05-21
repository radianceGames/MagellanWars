#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageReassignmentRemoveAddShipResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
		Message;
	Message.clear();

	ITEM("STRING_REMOVE_ADD_SHIPS", GETTEXT("Remove/Add Ships"));

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

	int
		ShipNeed = FleetNewSize-Fleet->get_current_ship();
	CDock *
		Pool = aPlayer->get_dock();

	if(Pool->count_ship(Fleet->get_design_id()) < ShipNeed)
	{
		CString
			ClassInfoURL;
		ClassInfoURL.clear();
		ClassInfoURL.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
							Fleet->get_design_id(),
							Fleet->CShipDesign::get_name());

		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You don't have additional %1$s %2$s ships."),
								dec2unit(ShipNeed),
								(char *)ClassInfoURL));
		return output("fleet/reassignment_error.html");
	}

	Fleet->set_max_ship(FleetNewSize);
	Fleet->set_current_ship(FleetNewSize);
	aPlayer->get_dock()->change_ship((CShipDesign *)Fleet, -ShipNeed);

	aPlayer->get_fleet_list()->refresh_power();	//telecard 2001/06/14
	aPlayer->refresh_power();

	CString
		ClassInfoURL;
	ClassInfoURL.clear();
	ClassInfoURL.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
						Fleet->get_design_id(),
						Fleet->CShipDesign::get_name());

	Message.format(GETTEXT("You have successfully changed %1$s as a fleet of %2$s %3$s."),
						Fleet->get_name(),
						dec2unit(FleetNewSize),
						(char *)ClassInfoURL);

	int ShipExpDif = FleetNewSize - Fleet->get_max_ship(); 
	if (ShipExpDif > 0)
 	{
 		int BaseExp = 25 + aPlayer->get_control_model()->get_military()*3; //the min exp the fleets should ever have
 		int OriginalExpWeight = Fleet->get_max_ship()*Fleet->get_exp();
 		int NewExpWeight = ShipExpDif*BaseExp;
 		int LostExp = Fleet->get_exp() - (int)((OriginalExpWeight+NewExpWeight)/FleetNewSize);
 		Fleet->change_exp(-LostExp);
 		Message += "<BR>\n";
 		Message.format(GETTEXT("%1$s has lost %2$s exp."),
 						Fleet->get_name(), dec2unit(LostExp));
 	}
	
	Fleet->type(QUERY_UPDATE);
	STORE_CENTER->store(*Fleet);

	ITEM("RESULT_MESSAGE", (char *)Message);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/reassignment_removeadd_ship_result.html");
}

