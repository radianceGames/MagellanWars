#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../player.h"
#include "../../preference.h"

bool
CPageDefensePlanSpecialChangeDeployment::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

   	// update preference
    
    	CPreference *aPreference = aPlayer->get_preference();
    
    	QUERY("CHANGE_PREF", ChangePrefString);
    	if (ChangePrefString)
    	{
	       int ChangePref = as_atoi(ChangePrefString);
       		aPreference->handle(ChangePref);
    	}

	static CString
		CapitalFleetURL,
		NonCapitalFleetURL;
	CapitalFleetURL.clear();
	CapitalFleetURL.format("%s/image/as_game/fleet/ship_cap.gif",
							(char *)CGame::mImageServerURL);
	NonCapitalFleetURL.clear();
	NonCapitalFleetURL.format("%s/image/as_game/fleet/ship_set.gif",
								(char *)CGame::mImageServerURL);

	QUERY("PLAN_ID", PlanIDString);
	int
		PlanID = as_atoi(PlanIDString);

	CDefensePlanList *
		DefensePlanList = aPlayer->get_defense_plan_list();
	CDefensePlan *
		DefensePlan = DefensePlanList->get_by_id(PlanID);

	if (!DefensePlan)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("That defense plan doesn't exist."));
		return output("war/defense_plan_special_error.html");
	}

	CFleetList *
		FleetList = aPlayer->get_fleet_list();

	bool
		AnyFleet[11][31];

	memset(AnyFleet, 0, sizeof(AnyFleet));

	QUERY("CAPITAL", CapitalString)
	int
		CapitalID = as_atoi(CapitalString);
	if (!CapitalID)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select a capital fleet."));
		return output("war/defense_plan_special_error.html");
	}

	CIntegerList
		FleetIDList;

	CFleet *
		CapitalFleet = FleetList->get_by_id(CapitalID);
	if (!CapitalFleet)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You don't have such a capital fleet."));
		return output("war/defense_plan_special_new_error.html");
	}

	FleetIDList.insert(0, (void *)CapitalID);

	for (int i=0 ; i<FleetList->length() ; i++)
	{
		CString
			QueryVar;
		QueryVar.clear();

		QueryVar.format("FLEET%d", i);

		QUERY((char *)QueryVar, FleetNumberString);
		if (!FleetNumberString) continue;
		if (strcasecmp(FleetNumberString, "ON")) continue;

		CFleet *
			Fleet = (CFleet *)FleetList->get(i);

		if (Fleet->get_id() == CapitalID) continue;

		FleetIDList.insert(FleetIDList.length(), (void *)Fleet->get_id());
	}

	if (!FleetIDList.length())
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You must select at least one fleet."));
		return output("war/defense_plan_special_new_error.html");
	}

	bool
		IsThereCapital = false;
	for (int i=0 ; i<FleetIDList.length() ; i++)
	{
		int
			FleetID = (int)FleetIDList.get(i);
		if (FleetID != CapitalID) continue;
		IsThereCapital = true;
		break;
	}

	if (!IsThereCapital)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("The capital fleet must be deployed."
										" Please check the one you have selected"
										" to deploy in the field."));
		return output("war/defense_plan_special_error.html");
	}

	ITEM("STRING_BATTLE_FIELD", GETTEXT("Battle Field"));

	ITEM("STRING_ORDER", GETTEXT("Order"));
	ITEM("STRING_NORMAL", GETTEXT("Normal"));
	ITEM("STRING_FORMATION", GETTEXT("Formation"));
	ITEM("STRING_PENETRATE", GETTEXT("Penetrate"));
	ITEM("STRING_FLANK", GETTEXT("Flank"));
	ITEM("STRING_RESERVE", GETTEXT("Reserve"));
	ITEM("STRING_ASSAULT", GETTEXT("Assault"));
	ITEM("STRING_FREE", GETTEXT("Free"));
	ITEM("STRING_STAND_GROUND", GETTEXT("Stand Ground"));

	ITEM("STRING_DESCRIPTION", GETTEXT("Description"));

	ITEM("FLEET_NUMBER", FleetIDList.length());

	CString FleetIndexToID;

	FleetIndexToID.clear();

	FleetIndexToID.format("<INPUT TYPE=hidden NAME=capFleet_id VALUE=%d>",
							FleetIDList.get(0));
	FleetIndexToID += "\n";

	for (int i=1 ; i<FleetIDList.length() ; i++)
	{
		FleetIndexToID.format("<INPUT TYPE=hidden NAME=fleet%d_id VALUE=%d>",
								i+1, FleetIDList.get(i));
		FleetIndexToID += "\n";
	}

	ITEM("FLEET_INDEX_TO_ID", (char *)FleetIndexToID);

	CString
		JSFleetInfo,
		JSFleetList,
		Result;

	JSFleetInfo = "<DIV ID=capFleet CLASS=pointer_h>";

	CDefenseFleetList *
		DefenseFleetList = DefensePlan->get_fleet_list();
	CDefenseFleet *
		CapitalDefenseFleet = DefenseFleetList->get_by_id(CapitalID);
	if (CapitalDefenseFleet)
	{
		int
			Command = CapitalDefenseFleet->get_command();

		JSFleetInfo.format("<IMG NAME=\"img_fleet%d\" SRC=\"%s\""
							" ALT=\"- %s -&#13;%s %d&#13;%s %s\""
							" TITLE=\"- %s -&#13;%s %d&#13;%s %s\">",
							CapitalID,
							(char *)CapitalFleetURL,
							GETTEXT("Fleet Info."),
							GETTEXT("No."),
							CapitalID,
							GETTEXT("Order"),
							CDefenseFleet::get_command_string_normal(Command),
							GETTEXT("Fleet Info."),
							GETTEXT("No."),
							CapitalID,
							GETTEXT("Order"),
							CDefenseFleet::get_command_string_normal(Command));
	} else
	{
		JSFleetInfo.format("<IMG NAME=\"img_fleet%d\" SRC=\"%s\""
							" ALT=\"- %s -&#13;%s %d&#13;%s %s\""
							" TITLE=\"- %s -&#13;%s %d&#13;%s %s\">",
							CapitalID,
							(char *)CapitalFleetURL,
							GETTEXT("Fleet Info."),
							GETTEXT("No."),
							CapitalID,
							GETTEXT("Order"),
							CDefenseFleet::get_command_string_normal(CDefenseFleet::COMMAND_NORMAL),
							GETTEXT("Fleet Info."),
							GETTEXT("No."),
							CapitalID,
							GETTEXT("Order"),
							CDefenseFleet::get_command_string_normal(CDefenseFleet::COMMAND_NORMAL));
	}

	JSFleetInfo += "</DIV>";
	JSFleetInfo += "\n";

	JSFleetList = "<STYLE TYPE=\"text/css\">\n";
	JSFleetList += "#capFleet{position:absolute; left:303; top:316; z-index:0}";
	JSFleetList += "\n";

	Result = "<INPUT TYPE=hidden NAME=capFleet_O VALUE=\"NORMAL\">";
	Result += "\n";

	AnyFleet[5][15] = true;

	for (int i=1 ; i<FleetIDList.length() ; i++)
	{
		int
			FleetID = (int)FleetIDList.get(i);

		CDefenseFleetList *
			DefenseFleetList = DefensePlan->get_fleet_list();
		CDefenseFleet *
			DefenseFleet = (CDefenseFleet *)DefenseFleetList->get_by_id(FleetID);

		if (!DefenseFleet) continue;

		if (DefenseFleet->get_fleet_id() == DefensePlan->get_capital()) continue;
		if (DefenseFleet->get_fleet_id() == CapitalID) continue;

		int
			LocationX = (DefenseFleet->get_y() - 2000)/10 + 9,
			LocationY = (DefenseFleet->get_x() - 7000)/10 + 226;
		int
			Command = DefenseFleet->get_command();

		JSFleetList.format("#%s%d{position:absolute; left:%d; top:%d; z-index:%d}",
							"fleet", i+1, LocationX - 5, LocationY - 8, i);
		JSFleetList += "\n";

		Result.format("<INPUT TYPE=hidden NAME=fleet%d_X VALUE=\"%d\">",
						i+1, LocationX);
		Result += "\n";
		Result.format("<INPUT TYPE=hidden NAME=fleet%d_Y VALUE=\"%d\">",
						i+1, LocationY);
		Result += "\n";
		Result.format("<INPUT TYPE=hidden NAME=fleet%d_O VALUE=\"%s\">",
						i+1, CDefenseFleet::get_command_string_upper_case(Command));
		Result += "\n";

		int
			Column = (LocationX - 9)/20,
			Row = (LocationY - 226)/20;

		AnyFleet[Row][Column] = true;

		JSFleetInfo.format("<DIV ID=%s%d CLASS=pointer_h>", "fleet", i+1);
		JSFleetInfo.format("<IMG NAME=\"img_fleet%d\" SRC=\"%s\""
							" ALT=\"- %s -&#13;%s %d&#13;%s %s\""
							" TITLE=\"- %s -&#13;%s %d&#13;%s %s\">",
							i+1,
							(char *)NonCapitalFleetURL,
							GETTEXT("Fleet Info."),
							GETTEXT("No."),
							FleetID,
							GETTEXT("Order"),
							CDefenseFleet::get_command_string_normal(Command),
							GETTEXT("Fleet Info."),
							GETTEXT("No."),
							FleetID,
							GETTEXT("Order"),
							CDefenseFleet::get_command_string_normal(Command));
		JSFleetInfo += "</DIV>";
		JSFleetInfo += "\n";
	}

	int
		LocationX = 609,
		LocationY = 226;

	for (int i=1 ; i<FleetIDList.length() ; i++)
	{
		int
			FleetID = (int)FleetIDList.get(i);
		CDefenseFleetList *
			DefenseFleetList = DefensePlan->get_fleet_list();
		CDefenseFleet *
			DefenseFleet = DefenseFleetList->get_by_id(FleetID);

		if (DefenseFleet)
		{
			if (DefenseFleet->get_fleet_id() != DefensePlan->get_capital() &&
				DefenseFleet->get_fleet_id() != CapitalID) continue;
		}

		int
			Column = (LocationX - 9)/20,
			Row = (LocationY - 226)/20;

		while (AnyFleet[Row][Column] == true)
		{
			if (Row == 10)
			{
				LocationX -= 20;
				LocationY = 226;
			} else LocationY += 20;

			Column = (LocationX - 9)/20;
			Row = (LocationY - 226)/20;
		}

		JSFleetList.format("#%s%d{position:absolute; left:%d; top:%d; z-index:%d}",
							"fleet", i+1, LocationX - 5, LocationY - 8, i);
		JSFleetList += "\n";

		Result.format("<INPUT TYPE=hidden NAME=fleet%d_X VALUE=\"%d\">",
						i+1, LocationX);
		Result += "\n";
		Result.format("<INPUT TYPE=hidden NAME=fleet%d_Y VALUE=\"%d\">",
						i+1, LocationY);
		Result += "\n";
		Result.format("<INPUT TYPE=hidden NAME=fleet%d_O VALUE=\"NORMAL\">", i+1);
		Result += "\n";

		AnyFleet[Row][Column] = true;

		JSFleetInfo.format("<DIV ID=%s%d CLASS=pointer_h>", "fleet", i+1);
		if (DefenseFleet)
		{
			int
				Command = DefenseFleet->get_command();

			JSFleetInfo.format("<IMG NAME=\"img_fleet%d\" SRC=\"%s\""
								" ALT=\"- %s -&#13;%s %d&#13;%s %s\""
								" TITLE=\"- %s -&#13;%s %d&#13;%s %s\">",
								i+1,
								(char *)NonCapitalFleetURL,
								GETTEXT("Fleet Info."),
								GETTEXT("No."),
								FleetID,
								GETTEXT("Order"),
								CDefenseFleet::get_command_string_normal(Command),
								GETTEXT("Fleet Info."),
								GETTEXT("No."),
								FleetID,
								GETTEXT("Order"),
								CDefenseFleet::get_command_string_normal(Command));
		} else
		{
			JSFleetInfo.format("<IMG NAME=\"img_fleet%d\" SRC=\"%s\""
								" ALT=\"- %s -&#13;%s %d&#13;%s %s\""
								" TITLE=\"- %s -&#13;%s %d&#13;%s %s\">",
								i+1,
								(char *)NonCapitalFleetURL,
								GETTEXT("Fleet Info."),
								GETTEXT("No."),
								FleetID,
								GETTEXT("Order"),
								CDefenseFleet::get_command_string_normal(CDefenseFleet::COMMAND_NORMAL),
								GETTEXT("Fleet Info."),
								GETTEXT("No."),
								FleetID,
								GETTEXT("Order"),
								CDefenseFleet::get_command_string_normal(CDefenseFleet::COMMAND_NORMAL));
		}
		JSFleetInfo += "</DIV>";
		JSFleetInfo += "\n";
	}


	JSFleetList += "</STYLE>\n";

	ITEM("JS_FLEET_LIST", (char *)JSFleetList);
	ITEM("RESULT", (char *)Result);
	ITEM("JS_FLEET_INFO", (char *)JSFleetInfo);

	ITEM("PLAN_ID", PlanID);

//	system_log( "end page handler %s", get_name() );

	return output("war/defense_plan_special_change_deployment.html");
}

