#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../player.h"
#include "../../preference.h"

bool
CPageDefensePlanSpecialNewDeployment::handler(CPlayer *aPlayer)
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

	QUERY("PLAN_NAME", PlanNameString);
	if (!PlanNameString)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select a name of new plan."));
		return output("war/defense_plan_special_error.html");
	}
	if (!is_valid_name((char *)PlanNameString))
	{
		ITEM("ERROR_MESSAGE", GETTEXT("That name is invalid."));
		return output("war/defense_plan_special_error.html");
	}

	CFleetList *
		FleetList = aPlayer->get_fleet_list();

	bool
		AnyFleet[11][31];

	for (int i=0 ; i<11 ; i++)
	{
		for (int j=0 ; j<31 ; j++)
		{
			AnyFleet[i][j] = false;
		}
	}

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
		return output("war/defense_plan_special_error.html");
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
		return output("war/defense_plan_special_error.html");
	}

	QUERY("RACE_CONDITION", RaceConditionString);
	ITEM("RACE_CONDITION", RaceConditionString);

	QUERY("POWER_MIN", PowerMinString);
	ITEM("POWER_MIN", PowerMinString);

	QUERY("POWER_MAX", PowerMaxString);
	ITEM("POWER_MAX", PowerMaxString);

	QUERY("ATTACK_TYPE_CONDITION", AttackTypeConditionString);
	ITEM("ATTACK_TYPE_CONDITION", AttackTypeConditionString);

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

	static CString
		FleetIndexToID;
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

	static CString
		JSFleetInfo,
		JSFleetList,
		Result;
	JSFleetInfo.clear();
	JSFleetList.clear();
	Result.clear();

	JSFleetInfo = "<DIV ID=capFleet CLASS=pointer_h>";

	JSFleetInfo.format("<IMG NAME=\"img_capFleet\" SRC=\"%s\""
						" ALT=\"- %s -&#13;%s %d&#13;%s %s\""
						" TITLE=\"- %s -&#13;%s %d&#13;%s %s\">",
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

	JSFleetInfo += "</DIV>";
	JSFleetInfo += "\n";

	JSFleetList = "<STYLE TYPE=\"text/css\">\n";
	JSFleetList += "#capFleet{position:absolute; left:303; top:316; z-index:0}";
	JSFleetList += "\n";

	Result = "<INPUT TYPE=hidden NAME=capFleet_O VALUE=\"NORMAL\">";
	Result += "\n";

	AnyFleet[5][15] = true;

	int
		LocationX = 609,
		LocationY = 226;

	for (int i=1 ; i<FleetIDList.length() ; i++)
	{
		int
			FleetID = (int)FleetIDList.get(i);

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

		JSFleetInfo += "</DIV>";
		JSFleetInfo += "\n";
	}

	JSFleetList += "</STYLE>\n";

	ITEM("JS_FLEET_LIST", (char *)JSFleetList);
	ITEM("RESULT", (char *)Result);
	ITEM("JS_FLEET_INFO", (char *)JSFleetInfo);

	ITEM("PLAN_NAME", PlanNameString);
	ITEM("FLEET_NUMBER", FleetIDList.length());

//	system_log( "end page handler %s", get_name() );

	return output("war/defense_plan_special_new_deployment.html");
}

