#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"

bool
CPageDefensePlanSpecialOld::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

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
		ITEM("ERROR_MESSAGE", GETTEXT("You selected the defense that doesn't exist."));
		return output("war/defense_plan_special_error.html");
	}

	QUERY("PLAN_BUTTON", PlanButtonString);

	if (!PlanButtonString)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You don't seem to had selected any button."));
		return output("war/defense_plan_special_error.html");
	}

	if (!strcmp(PlanButtonString, "DELETE"))
	{
		ITEM("STRING_RACE", GETTEXT("Race"));
		ITEM("RACE", DefensePlan->get_for_which_race());

		ITEM("STRING_POWER_RATIO", GETTEXT("Power Ratio"));
		ITEM("POWER_RATIO",
				(char *)format("%d%% - %d%%",
								DefensePlan->get_min(), DefensePlan->get_max()));

		ITEM("STRING_ATTACK_TYPE", GETTEXT("Attack Type"));
		ITEM("ATTACK_TYPE", DefensePlan->get_for_which_attack());

		ITEM("CONFIRM_MESSAGE",
				GETTEXT("Do you really want to delete the following defense plan?"));

		ITEM("PLAN_ID", PlanID);

//		system_log( "end page handler %s", get_name() );

		return output("war/defense_plan_special_delete.html");
	}
	else if (!strcmp(PlanButtonString, "CHANGE"))
	{
		CFleetList *
			FleetList = aPlayer->get_fleet_list();
		if (FleetList->fleet_number_by_status(CFleet::FLEET_STAND_BY) == 0)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You do not have any fleets on stand-by.<BR>"
							"Please try again later."));
			return output("war/defense_plan_special_error.html");
		}

		ITEM("SELECT_FLEET_MESSAGE",
				GETTEXT("Select fleets to deploy(up to 20 fleets)."));

		ITEM("FLEET_LIST", FleetList->deployment_fleet_list_html(aPlayer));

		ITEM("PLAN_ID", PlanID);

//		system_log( "end page handler %s", get_name() );

		return output("war/defense_plan_special_change.html");
	}
	else
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You selected wrong button."));
		return output("war/defense_plan_special_error.html");
	}
//		system_log( "end page handler %s", get_name() );
}

