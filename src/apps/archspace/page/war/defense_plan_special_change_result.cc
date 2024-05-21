#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageDefensePlanSpecialChangeResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	CDefensePlanList *
		DefensePlanList = aPlayer->get_defense_plan_list();

	QUERY("PLAN_ID", PlanIDString);
	int
		PlanID = as_atoi(PlanIDString);
	CDefensePlan *
		DefensePlan = DefensePlanList->get_by_id(PlanID);

	if (!DefensePlan)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You selected a plan that doesn't exist."));
		return output("war/defense_plan_special_error.html");
	}

	QUERY("FLEET_NUMBER", FleetNumberString)
	int
		FleetNumber = as_atoi(FleetNumberString);
	if (!FleetNumber)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Wrong fleet number received."
										" Please ask Archspace Development Team."));
		return output("war/defense_plan_special_error.html");
	}

	if (FleetNumber < 1 || FleetNumber > 20) {
 		ITEM("ERROR_MESSAGE",(char *)format(GETTEXT("Wrong fleet number received. Please ask Archspace Development Team.")));
 		return output("war/defense_plan_special_error.html");
 		}

	CDefensePlan
		TempDefensePlan;

	QUERY("capFleet_id", CapitalIDString)
	int
		CapitalID = as_atoi(CapitalIDString);
	if (!CapitalID)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select a capital fleet."));
		return output("war/defense_plan_special_error.html");
	}

	QUERY("capFleet_O", CommandString);
	int
		Command = CDefenseFleet::get_fleet_command_from_string((char *)CommandString);
	if (Command == -1)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Wrong command string received." 
										" Please ask Archspace Development Team."));
		return output("war/defense_plan_generic_new_error.html");
	}

	CDefenseFleet *
		DefenseFleet = new CDefenseFleet;

	DefenseFleet->set_owner(aPlayer->get_game_id());
	DefenseFleet->set_plan_id(PlanID);
	DefenseFleet->set_fleet_id(CapitalID);
	DefenseFleet->set_command(Command);
	DefenseFleet->set_x( (326 - 226)*10 + 7000 );
	DefenseFleet->set_y( (309 - 9)*10 + 2000 );

	TempDefensePlan.add_defense_fleet(DefenseFleet);

	for (int i=2 ; i<=FleetNumber ; i++)
	{
		CString
			QueryVar;
		QueryVar.clear();

		QueryVar.format("fleet%d_id", i);

		QUERY((char *)QueryVar, FleetIDString);

		int
			FleetID = as_atoi(FleetIDString);
		CFleet *
			Fleet = FleetList->get_by_id(FleetID);

		if (!Fleet)
		{
			ITEM("ERROR_MESSAGE", GETTEXT("Wrong fleet ID received."
											" Please ask Archspace Development Team."));

			return output("war/defense_plan_special_error.html");
		}

		QUERY((char *)format("fleet%d_X", i), LocationXString);
		QUERY((char *)format("fleet%d_Y", i), LocationYString);

		int
			LocationX = as_atoi(LocationXString);
		int
			LocationY = as_atoi(LocationYString);

		QUERY((char *)format("fleet%d_O", i), CommandString);
		int
			Command = CDefenseFleet::get_fleet_command_from_string((char *)CommandString);
		if (Command == -1)
		{
			ITEM("ERROR_MESSAGE", GETTEXT("Wrong command string received." 
											" Please ask Archspace Development Team."));
			return output("war/defense_plan_generic_new_error.html");
		}

		CDefenseFleet *
			DefenseFleet = new CDefenseFleet();
		DefenseFleet->set_owner(aPlayer->get_game_id());
		DefenseFleet->set_plan_id(PlanID);
		DefenseFleet->set_fleet_id(FleetID);
		DefenseFleet->set_command(Command);
		DefenseFleet->set_x( (LocationY - 226)*10 + 7000 );
		DefenseFleet->set_y( (LocationX - 9)*10 + 2000 );

		TempDefensePlan.add_defense_fleet(DefenseFleet);
	}

	if (TempDefensePlan.is_there_stacked_fleet() == true)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You stacked 1 or more fleets on another fleet(s)."));
		return output("war/defense_plan_error.html");
	}

	CDefenseFleetList
		*DefenseFleetList = DefensePlan->get_fleet_list(),
		*TempDefenseFleetList = TempDefensePlan.get_fleet_list();

	for (int i=DefenseFleetList->length()-1 ; i>=0 ; i--)
	{
		CDefenseFleet *
			DefenseFleet = (CDefenseFleet *)DefenseFleetList->get(i);

		DefenseFleet->type(QUERY_DELETE);
		STORE_CENTER->store(*DefenseFleet);

		DefenseFleetList->remove_defense_fleet(DefenseFleet->get_fleet_id());
	}

	for (int i=TempDefenseFleetList->length()-1 ; i>=0 ; i--)
	{
		CDefenseFleet *
			DefenseFleet = (CDefenseFleet *)TempDefenseFleetList->get(i);

		TempDefenseFleetList->remove_without_free_defense_fleet(DefenseFleet->get_fleet_id());
		DefenseFleetList->add_defense_fleet(DefenseFleet);

		DefenseFleet->type(QUERY_INSERT);
		STORE_CENTER->store(*DefenseFleet);
	}

	DefensePlan->set_capital(CapitalID);

	DefensePlan->type(QUERY_UPDATE);
	STORE_CENTER->store(*DefensePlan);

	ITEM("RESULT_MESSAGE",
			GETTEXT("You have successfully deployed your fleets for the defense plan."));

//	system_log( "end page handler %s", get_name() );

	return output("war/defense_plan_special_change_result.html");
}

