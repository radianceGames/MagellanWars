#include <libintl.h>
#include "../../pages.h"

bool
CPageReassignmentRemoveAddShipConfirm::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	ITEM("STRING_REMOVE_ADD_SHIPS", GETTEXT("Remove/Add Ships"));

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

	if (FleetNewSize <= 0)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You entered invalid value(%1$s) for the new number of ships."),
								dec2unit(FleetNewSize)));
		return output("fleet/reassignment_error.html");
	}

	CDock *
		Pool = aPlayer->get_dock();

	if (Pool->count_ship(Fleet->get_design_id()) <
		FleetNewSize - Fleet->get_current_ship())
	{
		CString
			ClassInfoURL;
		ClassInfoURL.clear();
		ClassInfoURL.format("<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\">%s</A>",
							Fleet->get_design_id(),
							Fleet->CShipDesign::get_name());

		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You don't have additional %1$s %2$s ships."),
								dec2unit(FleetNewSize-Fleet->get_current_ship()),
								(char *)ClassInfoURL));
		return output("fleet/reassignment_error.html");
	}

	ITEM("REASSIGNMENT_FLEET",
			(char *)format("<INPUT TYPE=hidden NAME=FLEET VALUE=%d>\n",
							Fleet->get_id()));
	ITEM("FLEET_NEW_SIZE",
			(char *)format("<INPUT TYPE=hidden NAME=FLEET_NEW_SIZE VALUE=%d>\n",
							FleetNewSize));
	ITEM("NEW_SIZE", FleetNewSize);
	ITEM("FLEET_NAME", Fleet->get_name());

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

	Message.format(GETTEXT("You have selected %1$s ship(s) of %2$s."),
					dec2unit(FleetNewSize), Fleet->CShipDesign::get_name());
	Message += "<BR>\n";
	Message.format(
			GETTEXT("If you add more ships, %1$s will lose some exp."),
			Fleet->get_name());
	Message += "<BR>\n";
	Message += GETTEXT("(This process won't let the fleet lose any exp.)");
	Message += "<BR><BR>\n";
	Message += GETTEXT("Are you sure you want to make the change?");

	ITEM("CONFIRM_MESSAGE", (char *)Message);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/reassignment_removeadd_ship_confirm.html");
}

