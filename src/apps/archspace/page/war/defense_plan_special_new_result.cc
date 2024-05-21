#include <libintl.h>
#include "../../pages.h"
#include "../../race.h"
#include "../../battle.h"
#include "../../archspace.h"

bool
CPageDefensePlanSpecialNewResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	QUERY("PLAN_NAME", PlanNameString);
	if (!PlanNameString)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select a name of plan."));
		return output("war/defense_plan_special_error.html");
	}
	if (!is_valid_name((char *)PlanNameString))
	{
		ITEM("ERROR_MESSAGE", GETTEXT("That name is invalid."));
		return output("war/defense_plan_special_error.html");
	}

	CFleetList *
		FleetList = aPlayer->get_fleet_list();

	QUERY("FLEET_NUMBER", FleetNumberString);
	int
		FleetNumber = as_atoi(FleetNumberString);

	if (FleetNumber < 1 || FleetNumber > 20) {
 		ITEM("ERROR_MESSAGE",(char *)format(GETTEXT("Wrong fleet number received. Please ask Archspace Development Team.")));
 		return output("war/defense_plan_special_error.html");
 		}

	CDefensePlanList *
		DefensePlanList = aPlayer->get_defense_plan_list();
	int
		PlanID = DefensePlanList->get_new_id();
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

	QUERY("RACE_CONDITION", RaceConditionString);
	CCommandSet
		Race;
	Race.set_string(RaceConditionString);

	int
		Enemy = 0;

	if (Race.has(CRace::RACE_HUMAN))
		Enemy += CDefensePlan::RACE_HUMAN;
	if (Race.has(CRace::RACE_TARGOID))
		Enemy += CDefensePlan::RACE_TARGOID;
	if (Race.has(CRace::RACE_BUCKANEER))
		Enemy += CDefensePlan::RACE_BUCKANEER;
	if (Race.has(CRace::RACE_TECANOID))
		Enemy += CDefensePlan::RACE_TECANOID;
	if (Race.has(CRace::RACE_EVINTOS))
		Enemy += CDefensePlan::RACE_EVINTOS;
	if (Race.has(CRace::RACE_AGERUS))
		Enemy += CDefensePlan::RACE_AGERUS;
	if (Race.has(CRace::RACE_BOSALIAN))
		Enemy += CDefensePlan::RACE_BOSALIAN;
	if (Race.has(CRace::RACE_XELOSS))
		Enemy += CDefensePlan::RACE_XELOSS;
	if (Race.has(CRace::RACE_XERUSIAN))
		Enemy += CDefensePlan::RACE_XERUSIAN;
	if (Race.has(CRace::RACE_XESPERADOS))
		Enemy += CDefensePlan::RACE_XESPERADOS;

	QUERY("POWER_MIN", PowerMinString);
	int
		PowerMin = as_atoi(PowerMinString);

	QUERY("POWER_MAX", PowerMaxString);
	int
		PowerMax = as_atoi(PowerMaxString);

	QUERY("ATTACK_TYPE_CONDITION", AttackTypeConditionString);
	CCommandSet
		AttackTypeCondition;
	AttackTypeCondition.set_string(AttackTypeConditionString);

	int
		AttackType = 0;

	if (AttackTypeCondition.has(CBattle::WAR_SIEGE))
		AttackType += CDefensePlan::TYPE_SIEGE;
	if (AttackTypeCondition.has(CBattle::WAR_BLOCKADE))
		AttackType += CDefensePlan::TYPE_BLOCKADE;

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
			Fleet = FleetList->get_by_id(i);

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
			DefenseFleet = new CDefenseFleet;
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

	CDefensePlan *
		NewDefensePlan = new CDefensePlan();
	NewDefensePlan->set_owner(aPlayer->get_game_id());
	NewDefensePlan->set_id(PlanID);
	NewDefensePlan->set_type(CDefensePlan::SPECIAL_PLAN);
	NewDefensePlan->set_enemy(Enemy);
	NewDefensePlan->set_min(PowerMin);
	NewDefensePlan->set_max(PowerMax);
	NewDefensePlan->set_attack_type(AttackType);
	NewDefensePlan->set_capital(CapitalID);
	NewDefensePlan->set_name((char *)PlanNameString);

	DefensePlanList->add_defense_plan(NewDefensePlan);

	NewDefensePlan->type(QUERY_INSERT);
	STORE_CENTER->store(*NewDefensePlan);

	CDefenseFleetList *
		TempDefenseFleetList = TempDefensePlan.get_fleet_list();

	for (int i=TempDefenseFleetList->length()-1 ; i>=0 ; i--)
	{
		CDefenseFleet *
			DefenseFleet = (CDefenseFleet *)TempDefenseFleetList->get(i);

		TempDefenseFleetList->remove_without_free_defense_fleet(DefenseFleet->get_fleet_id());
		NewDefensePlan->add_defense_fleet(DefenseFleet);

		DefenseFleet->type(QUERY_INSERT);
		STORE_CENTER->store(*DefenseFleet);
	}

	ITEM("RESULT_MESSAGE",
			GETTEXT("You have successfully deployed your fleets for the defense plan."));

//	system_log( "end page handler %s", get_name() );

	return output("war/defense_plan_special_new_result.html");
}

