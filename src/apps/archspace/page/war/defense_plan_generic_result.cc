#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../preference.h"
bool
CPageDefensePlanGenericResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );
    
	CDefensePlanList *
		DefensePlanList = aPlayer->get_defense_plan_list();
	CDefensePlan *
		GenericDefensePlan = DefensePlanList->get_generic_plan();
	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	CPreference *aPreference = aPlayer->get_preference();
	
	QUERY("FLEET_NUMBER", FleetNumberString)
	int
		FleetNumber = as_atoi(FleetNumberString);

	if (FleetNumber < 1 || FleetNumber > 20) {
 		ITEM("ERROR_MESSAGE",(char *)format(GETTEXT("Wrong fleet number received. Please ask Archspace Development Team.")));
 		return output("war/defense_plan_generic_new_error.html");
 		}
  
	int
		PlanID;
	if (GenericDefensePlan == NULL)
	{
		PlanID = DefensePlanList->get_new_id();
	}
	else
	{
		PlanID = GenericDefensePlan->get_id();
	}

	int	CapitalID;
	if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
    {   
        QUERY("capFleet_ID", CapitalIDString)
        CapitalID = as_atoi(CapitalIDString);        
    }
	else
	{
        QUERY("capFleet_id", CapitalIDString)
        CapitalID = as_atoi(CapitalIDString);        
    }

	CFleet *
		Fleet = (CFleet *)FleetList->get_by_id(CapitalID);

	if (!Fleet)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't select a capital fleet."));
		return output("war/defense_plan_generic_new_error.html");
	}

	QUERY("capFleet_O", CapitalCommandString);
	int
		Command = CDefenseFleet::get_fleet_command_from_string((char *)CapitalCommandString);
	if (Command == -1)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Wrong command string received." 
										" Please ask Archspace Development Team."));
		return output("war/defense_plan_generic_new_error.html");
	}
	CDefensePlan
		TempDefensePlan;

	CDefenseFleet *
		DefenseFleet = new CDefenseFleet;
//	SLOG("CAP FLEET ID: %d SET ORDER %d COMMAND STRING %s", CapitalID, Command, (char *)CapitalCommandString);
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

		if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
		    QueryVar.format("Fleet%d_ID", i);
		else
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

			return output("war/defense_plan_generic_new_error.html");
		}

		int
			LocationX;
		int
			LocationY;

		if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
		{
		  QUERY((char *)format("Fleet%d_X", i), LocationXString)
		  QUERY((char *)format("Fleet%d_Y", i), LocationYString)
		  LocationY = as_atoi(LocationYString);
		  LocationX = as_atoi(LocationXString);
		}
        else
        { 
          QUERY((char *)format("fleet%d_X", i), LocationXString)
          QUERY((char *)format("fleet%d_Y", i), LocationYString) 
		  LocationY = as_atoi(LocationYString);
		  LocationX = as_atoi(LocationXString);
        }
        
        int Command = -1;
		if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
		{
		  QUERY((char *)format("Fleet%d_O", i), CommandString)
		  Command = CDefenseFleet::get_fleet_command_from_string((char *)CommandString);
//		SLOG("FLEET ID: %d SET ORDER %d COMMAND STRING %s", FleetID, Command, (char *)CommandString);
  		}
        else
        {
          QUERY((char *)format("fleet%d_O", i), CommandString)
          Command = CDefenseFleet::get_fleet_command_from_string((char *)CommandString);
//		SLOG("FLEET ID: %d SET ORDER %d COMMAND STRING %s", FleetID, Command, (char *)CommandString);          
        }  
			
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

	if (GenericDefensePlan != NULL)
	{
		CDefenseFleetList *
			GenericDefenseFleetList = GenericDefensePlan->get_fleet_list();
		int
			CurrentFleetNumber = GenericDefenseFleetList->length();

		for (int i=CurrentFleetNumber-1 ; i>=0 ; i--)
		{
			CDefenseFleet *
				GenericDefenseFleet = (CDefenseFleet *)GenericDefenseFleetList->get(i);

			GenericDefenseFleet->type(QUERY_DELETE);
			*STORE_CENTER << *GenericDefenseFleet;

			GenericDefenseFleetList->remove_defense_fleet(GenericDefenseFleet->get_fleet_id());
		}
	}

	if (GenericDefensePlan == NULL)
	{
		GenericDefensePlan = new CDefensePlan();

		GenericDefensePlan->set_owner(aPlayer->get_game_id());
		GenericDefensePlan->set_id(PlanID);
		GenericDefensePlan->set_type(CDefensePlan::GENERIC_PLAN);
		GenericDefensePlan->set_capital(CapitalID);

		DefensePlanList->add_defense_plan(GenericDefensePlan);

		GenericDefensePlan->type(QUERY_INSERT);
		STORE_CENTER->store(*GenericDefensePlan);
	}
	else
	{
		GenericDefensePlan->set_capital(CapitalID);

		GenericDefensePlan->type(QUERY_UPDATE);
		STORE_CENTER->store(*GenericDefensePlan);
	}

	CDefenseFleetList *
		TempDefenseFleetList = TempDefensePlan.get_fleet_list();
	for (int i=TempDefenseFleetList->length()-1 ; i>=0 ; i--)
	{
		CDefenseFleet *
			DefenseFleet = (CDefenseFleet *)TempDefenseFleetList->get(i);

		TempDefenseFleetList->remove_without_free_defense_fleet(DefenseFleet->get_fleet_id());
		GenericDefensePlan->add_defense_fleet(DefenseFleet);

		DefenseFleet->type(QUERY_INSERT);
		STORE_CENTER->store(*DefenseFleet);
	}

	ITEM("RESULT_MESSAGE",
			GETTEXT("You have successfully deployed your fleets for the defense plan."));

//	system_log( "end page handler %s", get_name() );

	return output("war/defense_plan_generic_result.html");
}

