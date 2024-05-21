#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageFormNewFleetResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	static CString
			Message;
	Message.clear();

	CCommandSet
		AdmiralSet;
	AdmiralSet.clear();

	CAdmiralList *
		Pool = aPlayer->get_admiral_pool();

	int
		AdmiralIndex = -1;
	int
		PooledAdmirals = Pool->length();

	for (int i = 0 ; i < PooledAdmirals; i++)
	{
		AdmiralIndex++;
		char
			QueryFleetID[12] = "FLEET_ID000";
		if (i < 10)
		{
			QueryFleetID[8] = i + '0';
			QueryFleetID[9] = '\0';
		}
		else if (i < 100)
		{
			QueryFleetID[8] = i/10 + '0';
			QueryFleetID[9] = i%10 + '0';
			QueryFleetID[10] = '\0';
		}
		else if (i < 1000)
		{
			QueryFleetID[8] = i/100 + '0';
			QueryFleetID[9] = (i%100)/10 + '0';
			QueryFleetID[10] = i%10 + '0';
			QueryFleetID[11] = '\0';
		}
		else
		{
			ITEM("ERROR_MESSAGE", GETTEXT("Invalid admiral index.<br>Please try again."));
			return output("fleet/form_new_fleet_error.html");
		}

		QUERY(QueryFleetID, FleetIDString);
		if (!FleetIDString)
		{
			//Message += "Invalid FleetIDString. Please contact an Archspace developer.<BR>";
			continue; //Skip to next admiral.
		}

		char
			QueryFleetName[14] = "FLEET_NAME000";
		if (i < 10)
		{
			QueryFleetName[10] = i + '0';
			QueryFleetName[11] = '\0';
		}
		else if (i < 100)
		{
			QueryFleetName[10] = i/10 + '0';
			QueryFleetName[11] = i%10 + '0';
			QueryFleetName[12] = '\0';
		}
		else if (i < 1000)
		{
			QueryFleetName[10] = i/100 + '0';
			QueryFleetName[11] = (i%100)/10 + '0';
			QueryFleetName[12] = i%10 + '0';
			QueryFleetName[13] = '\0';
		}
		else
		{
			ITEM("ERROR_MESSAGE", GETTEXT("Invalid admiral index.<br>Please try again."));
			return output("fleet/form_new_fleet_error.html");
		}

		QUERY(QueryFleetName, FleetNameString);
		if (!FleetNameString)
		{
			//Message += "Invalid FleetNameString. Please contact an Archspace developer.<BR>";
			continue; //Skip to next admiral.
		}

		char
			QueryShipDesign[15] = "SHIP_DESIGN000";
		if (i < 10)
		{
			QueryShipDesign[11] = i + '0';
			QueryShipDesign[12] = '\0';
		}
		else if (i < 100)
		{
			QueryShipDesign[11] = i/10 + '0';
			QueryShipDesign[12] = i%10 + '0';
			QueryShipDesign[13] = '\0';
		}
		else if (i < 1000)
		{
			QueryShipDesign[11] = i/100 + '0';
			QueryShipDesign[12] = (i%100)/10 + '0';
			QueryShipDesign[13] = i%10 + '0';
			QueryShipDesign[14] = '\0';
		}
		else
		{
			ITEM("ERROR_MESSAGE", GETTEXT("Invalid admiral index.<br>Please try again."));
			return output("fleet/form_new_fleet_error.html");
		}

		QUERY(QueryShipDesign, ShipDesignString);
		if (!ShipDesignString)
		{
			//Message += "Invalid ShipDesignString. Please contact an Archspace developer.<BR>";
			continue; //Skip to next admiral.
		}

		char
			QueryShipNumber[15] = "SHIP_NUMBER000";
		if (i < 10)
		{
			QueryShipNumber[11] = i + '0';
			QueryShipNumber[12] = '\0';
		}
		else if (i < 100)
		{
			QueryShipNumber[11] = i/10 + '0';
			QueryShipNumber[12] = i%10 + '0';
			QueryShipNumber[13] = '\0';
		}
		else if (i < 1000)
		{
			QueryShipNumber[11] = i/100 + '0';
			QueryShipNumber[12] = (i%100)/10 + '0';
			QueryShipNumber[13] = i%10 + '0';
			QueryShipNumber[14] = '\0';
		}
		else
		{
			ITEM("ERROR_MESSAGE", GETTEXT("Invalid admiral index.<br>Please try again."));
			return output("fleet/form_new_fleet_error.html");
		}

		QUERY(QueryShipNumber, ShipNumberString);
		if (!ShipNumberString)
		{
			//Message += "Invalid ShipNumberString. Please contact an Archspace developer.<BR>";
			continue; //Skip to next admiral.
		}

		//woot we get to fleet something now!
		CAdmiral *
			Admiral = (CAdmiral *)Pool->get(AdmiralIndex);

		int
			FleetID = as_atoi(FleetIDString);

		if (FleetID <= 0)
		{
			Message.format("Invalid fleet ID selected for admiral %1$s.<BR>",
					Admiral->get_name());
			continue;
		}

		CFleetList *
			FleetList = aPlayer->get_fleet_list();
		CFleet *
			Fleet = (CFleet *)FleetList->get_by_id(FleetID);

		if (Fleet)
		{
			Message.format("Fleet ID selected for admiral %1$d already exists.<BR>",
								Admiral->get_id());
			continue;
		}

		CString
			FleetName;
		FleetName = FleetNameString;
		FleetName.htmlspecialchars();
		FleetName.nl2br();
		FleetName.strip_all_slashes();
		if ((char *)FleetName == NULL)
		{
			Message.format("You didn't enter a fleet name for fleet %1$d.<BR>",
					FleetID);
			continue;
		}

		if(!is_valid_name((char *)FleetName))
		{
			Message.format("You didn't enter a valid fleet name for fleet %1$d.<BR>",
					FleetID);
			continue;
		}

		int
			ClassID = as_atoi(ShipDesignString);
		CShipDesign *
			Class = aPlayer->get_ship_design_list()->get_by_id(ClassID);
		if (!Class)
		{
			Message.format("You didn't enter a valid ship design ID for fleet %1$d.<BR>",
								FleetID);
			continue;
		}

		int
			ShipNumber = as_atoi(ShipNumberString);

		if (ShipNumber <= 0)
		{
			Message.format("You entered an invalid number of ships for fleet %1$d.<BR>",
											FleetID);
			continue;
		}

		if (ShipNumber > aPlayer->get_dock()->count_ship(ClassID))
		{
			Message.format("You have only %1$d %2$s ship(s).<BR>",
					aPlayer->get_dock()->count_ship(ClassID), Class->get_name());
			continue;
		}

		if (ShipNumber > Admiral->get_fleet_commanding())
		{
			Message.format("Fleet Commander %1$s can command %2$d ship(s) at most.<BR>",
					Admiral->get_name(), Admiral->get_fleet_commanding());
			continue;
		}

		Fleet = new CFleet;
		Fleet->set_id(FleetID);
		Fleet->set_owner(aPlayer->get_game_id());
		Fleet->set_name((char *)FleetName);
		Fleet->set_admiral(Admiral->get_id());
		Fleet->set_ship_class(Class);
		Fleet->set_max_ship(ShipNumber);
		Fleet->set_current_ship(ShipNumber);
		Fleet->set_exp(25 + aPlayer->get_control_model()->get_military()*3);

		aPlayer->get_dock()->change_ship(Class, -ShipNumber);
		aPlayer->get_fleet_list()->add_fleet(Fleet);

		Fleet->type(QUERY_INSERT);
		*STORE_CENTER << *Fleet;

		Admiral->set_fleet_number(FleetID);
		Pool->remove_without_free_admiral(Admiral->get_id());
		aPlayer->get_admiral_list()->add_admiral(Admiral);

		Admiral->type(QUERY_UPDATE);
		*STORE_CENTER << *Admiral;

		AdmiralIndex--;

		Message.format("The new fleet %1$s has been formed. It is now on stand-by.<BR>",
							Fleet->get_nick());
	}

	ITEM("RESULT_MESSAGE", Message);

//	system_log( "end page handler %s", get_name() );

	return output("fleet/form_new_fleet_result.html");
}

