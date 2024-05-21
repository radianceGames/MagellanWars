#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../battle.h"
#include <cstdio>
#include "../../define.h"

bool
CPageEmpireInvadeFortressResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );
	if (!CGame::mUpdateTurn)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You cannot attack the empire until time has started.")));
		return output("empire/invade_empire_error.html");
	}    

#ifdef PROTECTION_BEGINNER
	if (aPlayer->get_protected_mode() == CPlayer::PROTECTED_BEGINNER)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You cannot attack the empire for it is protecting you")));
		return output("empire/invade_empire_error.html");
	}
#endif
	CPreference *aPreference = aPlayer->get_preference();
	int
		HomeClusterID = aPlayer->get_home_cluster_id();

	CMagistrateList *
		MagistrateList = EMPIRE->get_magistrate_list();
	CMagistrate *
		Magistrate = MagistrateList->get_by_cluster_id(HomeClusterID);

	if (Magistrate == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no magistrate in the cluster(#%1$s)."),
							dec2unit(HomeClusterID)));
		return output("empire/invade_empire_error.html");
	}
	if (Magistrate->is_dead() == false)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("The magistrate in the cluster(#%1$s) is still alive. So you can't attack The Empire's planets yet."),
							dec2unit(HomeClusterID)));
		return output("empire/invade_empire_error.html");
	}

	CSortedPlanetList *
		EmpirePlanetList = EMPIRE->get_empire_planet_list();
	if (EmpirePlanetList->length() > 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Not all the Empire' planets have taken by players yet. So you can't attack The Empire's fortress yet."));
		return output("empire/invade_empire_error.html");
	}

	QUERY("FORTRESS_KEY", FortressKeyString);
	int
		FortressKey = as_atoi(FortressKeyString);
	CFortressList *
		FortressList = EMPIRE->get_fortress_list();
	CFortress *
		Fortress = FortressList->get_by_key(FortressKey);
	if (Fortress == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The fortress which you have selected doesn't exist."));
		return output("empire/invade_empire_error.html");
	}
	if (Fortress->is_attackable(aPlayer) == false)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The fortress which you have selected can't be attacked by you."));
		return output("empire/invade_empire_error.html");
	}

	if (CEmpire::mEmpireInvasionLimit != 0)
	{
		CPlayerActionList *
			PlayerActionList = aPlayer->get_action_list();

		int
			InvasionHistory = 0;
		time_t
			EffectiveStartTime = CGame::get_game_time() - CEmpire::mEmpireInvasionLimitDuration;
		for (int i=0 ; i<PlayerActionList->length() ; i++)
		{
			CPlayerAction *
				PlayerAction = (CPlayerAction *)PlayerActionList->get(0);
			if (PlayerAction->get_type() != CAction::ACTION_PLAYER_EMPIRE_INVASION_HISTORY) continue;
			if (PlayerAction->get_start_time() > EffectiveStartTime) InvasionHistory++;
		}

		if (InvasionHistory >= CEmpire::mEmpireInvasionLimit)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You are trying to attack the empire too often. Take a little rest."));
			return output("empire/invade_empire_error.html");
		}
	}

	QUERY("FLEET_NUMBER", FleetNumberString)
	int
		FleetNumber = as_atoi(FleetNumberString);

	if (FleetNumber < 1 || FleetNumber > 20) {
 		ITEM("ERROR_MESSAGE",(char *)format(GETTEXT("Wrong fleet number received. Please ask Archspace Development Team.")));
 		return output("empire/invade_empire_error.html");
 		}

	aPlayer->change_empire_relation(-100);

	CBattle
		Battle(CBattle::WAR_FORTRESS, aPlayer, EMPIRE, (void *)Fortress);

	CAdmiralList
		*AttackerAdmiralList,
		*DefenderAdmiralList;
	CFleetList
		*AttackerFleetList,
		*DefenderFleetList;
	CDefensePlan *
		DefensePlan;
	int
		TargetID = Fortress->get_owner_id();

	if (TargetID == EMPIRE_GAME_ID)
	{
		AttackerAdmiralList = aPlayer->get_admiral_list(),
		DefenderAdmiralList = Fortress->get_admiral_list();

		AttackerFleetList = aPlayer->get_fleet_list(),
		DefenderFleetList = Fortress->get_fleet_list();

		DefensePlan = new CDefensePlan();
		DefensePlan->init_for_empire(DefenderFleetList, false);
	}
	else
	{
		CPlayer *
			TargetPlayer = PLAYER_TABLE->get_by_game_id(TargetID);
		if (TargetPlayer == NULL)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("The owner of this fortress doesn't exist. Please ask Archspace Development Team."));
			return output("empire/invade_empire_error.html");
		}

		AttackerAdmiralList = aPlayer->get_admiral_list(),
		DefenderAdmiralList = TargetPlayer->get_admiral_list();

		AttackerFleetList = aPlayer->get_fleet_list(),
		DefenderFleetList = TargetPlayer->get_fleet_list();

		CDefensePlanList *
			DefensePlanList = TargetPlayer->get_defense_plan_list();
		DefensePlan = DefensePlanList->get_optimal_plan(aPlayer->get_race(), aPlayer->get_power(), TargetPlayer->get_power(), CDefensePlan::TYPE_SIEGE);
	}

	CDefensePlan
		OffensePlan;
	OffensePlan.set_owner(aPlayer->get_game_id());

	int	CapitalFleetID;
	if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
    {   
        QUERY("capFleet_ID", CapitalIDString)
        CapitalFleetID = as_atoi(CapitalIDString);        
    }
	else
	{
        QUERY("capFleet_id", CapitalIDString)
        CapitalFleetID = as_atoi(CapitalIDString);        
    }
    
   	CFleetList *FleetList = aPlayer->get_fleet_list();
	CFleet *Fleet = (CFleet *)FleetList->get_by_id(CapitalFleetID);

	if(!Fleet)
 	{
		ITEM("ERROR_MESSAGE",
		         GETTEXT("You didn't select a capital fleet."));
		return output("empire/invade_empire_error.html");
	}


    if (AttackerFleetList->get_by_id(CapitalFleetID) == NULL)
    {
		ITEM("ERROR_MESSAGE",
				GETTEXT("Invalid capital fleet ID number!"));
		return output("empire/invade_empire_error.html");
    }

	QUERY("capFleet_O", CapitalFleetCommandString);
	int
		CapitalFleetCommand = CDefenseFleet::get_fleet_command_from_string((char *)CapitalFleetCommandString);
	if (CapitalFleetCommand < CDefenseFleet::COMMAND_NORMAL ||
		CapitalFleetCommand >= CDefenseFleet::COMMAND_MAX)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The capital fleet has wrong command. Please ask Archspace Development Team."));
		return output("empire/invade_empire_error.html");
	}
	
	if(AttackerFleetList->get_by_id(CapitalFleetID)->get_status() != CFleet::FLEET_STAND_BY)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("One or more of the fleet(s) you sent were not on standby status."));

		return output("empire/invade_empire_error.html");
	}

	OffensePlan.set_capital(CapitalFleetID);

	CDefenseFleet *
		CapitalOffenseFleet = new CDefenseFleet;
	CapitalOffenseFleet->set_owner(aPlayer->get_game_id());
	CapitalOffenseFleet->set_fleet_id(CapitalFleetID);
	CapitalOffenseFleet->set_command(CapitalFleetCommand);
	CapitalOffenseFleet->set_x(2000);
	CapitalOffenseFleet->set_y(5000);

	OffensePlan.add_defense_fleet(CapitalOffenseFleet);

	for (int i=2 ; i<=FleetNumber ; i++)
	{
		char Query[100];
		
		sprintf(Query, "fleet%d_O", i);
		QUERY(Query, CommandString);

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
		  CHECK(!LocationXString ||	!LocationYString, GETTEXT("Missing Fleet Location Data"));
		}
        else
        { 
          QUERY((char *)format("fleet%d_X", i), LocationXString)
          QUERY((char *)format("fleet%d_Y", i), LocationYString) 
		  LocationY = as_atoi(LocationYString);
		  LocationX = as_atoi(LocationXString);
		  CHECK(!LocationXString ||	!LocationYString, GETTEXT("Missing Fleet Location Data"));
        };

		if (aPreference != NULL && aPreference->has(CPreference::PR_JAVA))
		    sprintf(Query, "Fleet%d_ID", i);
		else
            sprintf(Query, "fleet%d_id", i);      
		QUERY(Query, FleetIDString);


		CHECK(!CommandString || !FleetIDString,
				GETTEXT("1 or more fleets' deployment data were not found."));

		int
			Command = CDefenseFleet::get_fleet_command_from_string((char *)CommandString),
			FleetID = as_atoi(FleetIDString);

		CFleet *
			Fleet = (CFleet *)AttackerFleetList->get_by_id(FleetID);

		if (Fleet == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You don't have the fleet with ID %1$s."),
									dec2unit(FleetID)));
			return output("empire/invade_empire_error.html");
		}

		if(Fleet->get_status() != CFleet::FLEET_STAND_BY)
		{
			ITEM("ERROR_MESSAGE",
						GETTEXT("One or more of the fleet(s) you sent were not on standby status."));

			return output("empire/invade_empire_error.html");
		}

		CHECK(Command < CDefenseFleet::COMMAND_NORMAL ||
			Command >= CDefenseFleet::COMMAND_MAX,
					(char *)format(GETTEXT("The fleet %1$s has a wrong command."),
									Fleet->get_name()));

		CHECK(LocationX < 9 || LocationX > 609 ||
			LocationY < 226 || LocationY > 426,
			format(GETTEXT("%1$s has wrong location in the battle field."), Fleet->get_nick()));

#define SHIP_SIZE_CRUISER	5
		if (Fleet->get_size() < SHIP_SIZE_CRUISER)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("The size of %1$s is too small to voyage out to the Empire's cluster."), Fleet->get_nick()));

			return output("empire/invade_empire_error.html");
		}
#undef SHIP_SIZE_CRUISER

		CDefenseFleet *
			OffenseFleet = new CDefenseFleet;
		OffenseFleet->set_owner(aPlayer->get_game_id());
		OffenseFleet->set_fleet_id(FleetID);
		OffenseFleet->set_command(Command);
		OffenseFleet->set_x(3000-(LocationY - 226)*10);
		OffenseFleet->set_y(8000-(LocationX - 9)*10);

		OffensePlan.add_defense_fleet(OffenseFleet);
	}

	if (OffensePlan.is_there_stacked_fleet() == true)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You stacked 1 or more fleets on another fleet(s)."));
		return output("empire/invade_empire_error.html");
	}

	CString Report;
	CString BattleView;

	Report.format(GETTEXT("Your fleet(s) arrived at %1$s."), Fortress->get_name());
	Report += "<BR>\n";
	Report += GETTEXT("Your fleet(s) started detecting enemy fleets.");
	Report += "<BR>\n";

	if (Battle.init_battle_fleet(&OffensePlan, AttackerFleetList, AttackerAdmiralList, DefensePlan, DefenderFleetList, DefenderAdmiralList))
	{
		BattleView.format("%s (%s %d : %d)\n",
				GETTEXT("Fleet Battle View"),
				GETTEXT("Attack vs. Defense"),
				Battle.get_offense_battle_fleet_list().length(),
				Battle.get_defense_battle_fleet_list().length());

		CString Buf;

		Buf = "<PRE>\nBATTLE REPORT\n";
		while(Battle.run_step());
		Buf += Battle.get_result_report();
		Buf += "</PRE>\n";
		Report += Buf;

		Buf.clear();
		if (Battle.attacker_win())
		{
			Report += GETTEXT("You won the fleet battle on the orbit.");
			Report += "<BR>\n";

			Buf.format(GETTEXT("%1$s invaded %2$s and won."), 
					aPlayer->get_nick(), EMPIRE->get_name());
			Buf += "<BR>\n";

			aPlayer->time_news((char *)Buf);

			Fortress->set_owner_id(aPlayer->get_game_id());

			Fortress->type(QUERY_UPDATE);
			STORE_CENTER->store(*Fortress);

			Report.format(GETTEXT("You have gained %1$s."),
							Fortress->get_name());
			Report += "<BR>\n";

			ADMIN_TOOL->add_invade_fortress_log(
					(char *)format("The player %s has succeeded in invasion against the fortress(%d/%d/%d)",
									aPlayer->get_nick(),
									Fortress->get_layer(),
									Fortress->get_sector(),
									Fortress->get_order()));
		}
		else
		{
			//Report += GETTEXT("Your fleet had critical damage in the fleet battle.");
			//Report += "<BR>\n";
			//Report += GETTEXT("Your fleet has been destroyed completely.");

			Report.format(GETTEXT("You have failed to invade %1$s."), Fortress->get_name());

			Buf.format(GETTEXT("%1$s invaded %2$s and lost."),
					aPlayer->get_nick(), Fortress->get_name());
			Buf += "<BR>\n";

			aPlayer->time_news((char *)Buf);

			ADMIN_TOOL->add_invade_fortress_log(
					(char *)format("The player %s has failed in invasion against the fortress(%d/%d/%d)",
									aPlayer->get_nick(),
									Fortress->get_layer(),
									Fortress->get_sector(),
									Fortress->get_order()));
		}

		Battle.finish_report_after_battle();
		Battle.update_fleet_after_battle();
		Battle.save();

		BattleView.format("&nbsp;\n"
			"<A HREF=\"../war/battle_report_detail.as?LOG_ID=%d\">"
				"<IMG SRC=\"%s/image/as_game/bu_view.gif\""
					"WIDTH=44 HEIGHT=11 BORDER=\"0\"></A>",
					Battle.get_record()->get_id(),
					(char *)CGame::mImageServerURL);
	}
	else
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You don't have any stand-by fleets."));
		return output("empire/invade_empire_error.html");
	}

	ITEM("MESSAGE", Report);
	ITEM("BATTLE_VIEW", BattleView);

#ifdef PROTECTION_24HOURS
	if(aPlayer->get_protected_mode() == CPlayer::PROTECTED_24HOURS)
	{
		aPlayer->set_protected_mode(CPlayer::PROTECTED_NONE);
	}
#endif

	CDefenseFleetList *
		OffensePlanFleetList = OffensePlan.get_fleet_list();
	OffensePlanFleetList->remove_all();

	if (TargetID == EMPIRE_GAME_ID)
	{
		CDefenseFleetList *
			DefensePlanFleetList = DefensePlan->get_fleet_list();
		DefensePlanFleetList->remove_all();
	}

	for (int i=1 ; i<=FleetNumber ; i++)
	{
		int
			FleetID;

		if (i == 1)
		{
			FleetID = CapitalFleetID;
		}
		else
		{
			char Query[100];

			sprintf(Query, "fleet%d_id", i);
			QUERY(Query, FleetIDString);

			FleetID = as_atoi(FleetIDString);
		}

		CFleet *
			Fleet = (CFleet *)AttackerFleetList->get_by_id(FleetID);

		if (Fleet)
		{
			CEngine *
				Engine = (CEngine *)COMPONENT_TABLE->get_by_id(Fleet->get_engine());
			int
				ReturnTime = (9-Engine->get_level())*CGame::mSecondPerTurn;
			if (!aPlayer->has_cluster(EMPIRE_CLUSTER_ID))
				ReturnTime *= 2;    
			if (aPlayer->has_ability(ABILITY_FAST_MANEUVER))
			{
				ReturnTime = ReturnTime * 70 / 100;
			}
			ReturnTime = aPlayer->calc_PA(ReturnTime, CPlayerEffect::PA_CHANGE_YOUR_FLEET_RETURN_TIME);
			Fleet->init_mission(CMission::MISSION_RETURNING, 0);
			CMission &Mission = Fleet->get_mission();
			Mission.set_terminate_time(CGame::get_game_time() + ReturnTime);
		}
	}

	CPlayerActionEmpireInvasionHistory *
		InvasionHistory = new CPlayerActionEmpireInvasionHistory();
	InvasionHistory->set_owner(aPlayer);
	InvasionHistory->set_start_time(CGame::get_game_time());

	InvasionHistory->type(QUERY_INSERT);
	STORE_CENTER->store(*InvasionHistory);

	aPlayer->add_action(InvasionHistory);
	PLAYER_ACTION_TABLE->add_action(InvasionHistory);

//	system_log( "end page handler %s", get_name() );

	return output("empire/invade_fortress_result.html");
}

