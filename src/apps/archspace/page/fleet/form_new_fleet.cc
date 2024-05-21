#include <libintl.h>
#include "../../pages.h"

bool
CPageFormNewFleet::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );
 	// if player doesn't have a preference object yet, make it
   	if (aPlayer->get_preference() == NULL)
       	aPlayer->set_preference(new CPreference(aPlayer->get_game_id()));

	static CString
		Message1,
		Message2;
	Message1.clear();
	Message2.clear();

	ITEM("STRING_FLEET_NUMBER", GETTEXT("Fleet Number"));

	CAdmiralList *
		AdmiralPool = aPlayer->get_admiral_pool();

	if (!AdmiralPool)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Admiral Pool is NULL."
						" Please ask Archspace Development Team."));
		return output("fleet/form_new_fleet_error.html");
	}
	if(!AdmiralPool->length())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You don't have any admirals."));
		return output("fleet/form_new_fleet_error.html");
	}

	ITEM("FLEET_COMMANDER_JAVASCRIPT", AdmiralPool->fleet_commander_list_javascript(aPlayer));

	CDock *
		ShipPool = aPlayer->get_dock();

	if (!ShipPool->length())
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You don't have any ships."));
		return output("fleet/form_new_fleet_error.html");
	}

	CFleetList *
		FleetList = aPlayer->get_fleet_list();

	int
		FleetID = FleetList->get_new_fleet_id();
	ITEM("FLEET_ID", FleetID);

	ITEM("STRING_FLEET_NAME", GETTEXT("Fleet Name"));

	ITEM("STRING_PATROL", GETTEXT("Patrol"));
	ITEM("STRING_RESERVE", GETTEXT("Reserve"));
	ITEM("STRING_MOBILE", GETTEXT("Mobile"));
	ITEM("STRING_SCOUT", GETTEXT("Scout"));
	ITEM("STRING_EXPEDITION", GETTEXT("Expedition"));
	ITEM("STRING_SUPPORT", GETTEXT("Support"));
	ITEM("STRING_STRIKE", GETTEXT("Strike"));
	ITEM("STRING_STRATEGIC", GETTEXT("Strategic"));
	ITEM("STRING_ASSAULT", GETTEXT("Assault"));
	ITEM("STRING_GUARD", GETTEXT("Guard"));

	ITEM("STRING_FLEET_COMMANDER_S__IN_THE_POOL",
			GETTEXT("Fleet Commander(s) in the Pool"));

	ITEM("FLEET_COMMANDER_LIST", AdmiralPool->fleet_commander_list_html());

	ITEM("STRING_SHIP_CLASS", GETTEXT("Ship Class"));
	ITEM("STRING___OF_SHIPS", GETTEXT("# of Ship(s)"));
	ITEM("STRING_MAX_", GETTEXT("Max."));

	ITEM("SHIP_CLASS", ShipPool->print_html_select());

	bool
		Comma = false;
	int
		InitShipNumber;
	Message1 = "(";

	for (int i=0 ; i<ShipPool->length() ; i++)
	{
		CDockedShip *
			DockedShip = (CDockedShip *)ShipPool->get(i);

		if (i == 0) InitShipNumber = DockedShip->get_number();

		if (Comma) Message1 += ", ";
		Message1.format("%d", DockedShip->get_number());
		Comma = true;
	}
	Message1 += ", 0)";

	Comma = false;
	int
		InitFleetCommanding;
	Message2 = "(";

	for (int i=0 ; i<AdmiralPool->length() ; i++)
	{
		CAdmiral *
			Admiral = (CAdmiral *)AdmiralPool->get(i);

		if (i == 0) InitFleetCommanding = Admiral->get_fleet_commanding();

		if (Comma) Message2 += ", ";
		Message2.format("%d", Admiral->get_fleet_commanding());
		Comma = true;
	}
	Message2 += ")";

	ITEM("JS_SHIP_NUMBER_LIST", (char *)Message1);
	ITEM("JS_FLEET_COMMANDING_LIST", (char *)Message2);
	ITEM("JS_FLEET_COMMANDER_NUMBER", AdmiralPool->length());

	if (InitShipNumber > InitFleetCommanding)
	{
		ITEM("MAX_SHIP_NUMBER", InitFleetCommanding);
	} else
	{
		ITEM("MAX_SHIP_NUMBER", InitShipNumber);
	}

	ITEM("STRING_CURRENTLY_POSSESSED_FLEET_S_",
			GETTEXT("Currently Possessed Fleet(s)"));
	ITEM("POSSESSED_FLEET_INFO", FleetList->possessed_fleet_info_html());

//	system_log( "end page handler %s", get_name() );

	return output("fleet/form_new_fleet.html");
}

