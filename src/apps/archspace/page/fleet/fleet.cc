#include <libintl.h>
#include "../../pages.h"

bool
CPageFleet::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

    QUERY("ALLIED", AlliedResult);
    
    if (!AlliedResult) {
        // Do nothing
        ITEM("ALLIED_RESULT", GETTEXT(" "))
    } else {
        ITEM("ALLIED_RESULT", GETTEXT("Successfully tried to send back allied"))
    }

	ITEM("STRING_SHIPS_IN_THE_POOL", GETTEXT("Ships in the Pool"));

	CString
		buf;

	buf.clear();
	CDock *
		dock;
	dock = aPlayer->get_dock();
	if (!dock)
		system_log("CPageFleet : dock null");
	for (int i=0 ; i<dock->length() ; i++)
	{	CDockedShip *
			dockedShip;
		dockedShip = (CDockedShip*)dock->get(i);
		if (i == 0)
			buf.format( "<A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\"> %s(%d) </A>\n", dockedShip->get_design_id(), dockedShip->get_name(), dockedShip->get_number() );
		else
			buf.format( ", <A HREF=\"/archspace/fleet/class_specification.as?DESIGN_ID=%d\"> %s(%d) </A>\n", dockedShip->get_design_id(), dockedShip->get_name(), dockedShip->get_number() );
	}
	if (!dock->length())
	{
		buf = GETTEXT("You don't have any ships in the pool.");
	}
	ITEM("SHIPS", buf);

	ITEM("STRING_FLEET_STATUS", GETTEXT("Fleet Status"));

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	ITEM ("FLEETS", FleetList->all_fleet_list_html(aPlayer));

	ITEM("STRING_FLEETS_FROM_ALLIES", GETTEXT("Fleets From Allies"));

	CFleetList *
		AllyFleetList = aPlayer->get_ally_fleet_list();
	ITEM("FLEETS_FROM_ALLIES", AllyFleetList->all_ally_fleet_list_html());

	ITEM("SHIP_DESIGN_EXPLAIN",
			GETTEXT("You can design a new ship class."));
	ITEM("SHIP_BUILDING_EXPLAIN",
			GETTEXT("You can build ships here."));
	ITEM("FLEET_COMMANDER_EXPLAIN",
			GETTEXT("You can manage your fleet commanders."));
	ITEM("SHIP_POOL_EXPLAIN",
			GETTEXT("You can control your ships."));
	ITEM("FORM_NEW_FLEET_EXPLAIN",
			GETTEXT("You can form a new fleet."));
	ITEM("REASSIGNMENT_EXPLAIN",
			GETTEXT("You can change your fleet commander, number of ships in a fleet,"
					" or the ship class of a fleet."));
	ITEM("EXPEDITION_EXPLAIN",
			GETTEXT("You can send your fleet to discover a new world."));
	ITEM("MISSION_EXPLAIN",
			GETTEXT("You can send your fleets on a mission."));
	ITEM("RECALL_EXPLAIN",
			GETTEXT("You can recall your fleets stationed on planets or dispatched to allies."));
	ITEM("DISBAND_EXPLAIN",
			GETTEXT("You can disband your fleets and return the ships to the pool."));

//	system_log("end page handler %s", get_name());

	return output( "fleet/fleet.html" );
}

