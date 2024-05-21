#include <libintl.h>

#include "../../pages.h"

#include "../../archspace.h"

#include "../../game.h"

#include "../../battle.h"

#include <cstdio>

#include "../../define.h"



bool

	CPageEmpireInvadeMagistrateResult::handler(CPlayer *aPlayer)

{

 

//	ITEM("ERROR_MESSAGE",
//		(char *)format(GETTEXT("Your fleets cower before the empire's might, and your admirals ignore their orders to attack. Your invasion has been halted. Your people dare not defy the might of Jo'Keric or his minions Cyphtor, Naru, DT, and HippieHunter. They may regain their confidence at a later date, but for now they are mere cowards.")));
//		return output("empire/invade_empire_error.html");

	system_log( "start page handler %s", get_name() );

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

	CMagistrateList *

	MagistrateList = EMPIRE->get_magistrate_list();



	int

		ClusterID = aPlayer->get_home_cluster_id();

	CMagistrate *

	Magistrate = MagistrateList->get_by_cluster_id(ClusterID);



	if (Magistrate == NULL)

	{

		ITEM("ERROR_MESSAGE",

			(char *)format(GETTEXT("There is no magistrate in the cluster with ID %1$s."),

				dec2unit(ClusterID)));

		return output("empire/invade_empire_error.html");

	}

	if (Magistrate->is_dead() == true)

	{

		ITEM("ERROR_MESSAGE",

			GETTEXT("You can't attack the magistrate because he is already defeated."));

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





	aPlayer->change_empire_relation(-50);



	CPlanet *

	TargetPlanet = Magistrate->get_target_planet();



	CBattle

		Battle(CBattle::WAR_MAGISTRATE, aPlayer, EMPIRE, (void *)TargetPlanet);



	CAdmiralList

		*AttackerAdmiralList = aPlayer->get_admiral_list(),

	*DefenderAdmiralList;

	CFleetList

		*AttackerFleetList = aPlayer->get_fleet_list(),

	*DefenderFleetList;



	if (Magistrate->get_number_of_planets() <= 1)

	{

		DefenderAdmiralList = Magistrate->get_admiral_list();

		DefenderFleetList = Magistrate->get_fleet_list();

	}

	else

	{

		DefenderAdmiralList = EMPIRE->get_admiral_list();

		DefenderFleetList = EMPIRE->get_fleet_list();



		DefenderAdmiralList->remove_all();

		DefenderFleetList->remove_all();



		DefenderFleetList->create_as_empire_fleet_group_volatile(DefenderAdmiralList, CEmpire::EMPIRE_FLEET_GROUP_TYPE_MAGISTRATE, Magistrate->get_cluster_id());

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



	if (AttackerFleetList->get_by_id(CapitalFleetID) == NULL)

	{

		ITEM("ERROR_MESSAGE",

			GETTEXT("Invalid capital fleet ID number!"));

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



	Report.format(GETTEXT("Your fleet(s) arrived at the magistrate in the cluster (#%1$s)."), 

		dec2unit(ClusterID));

	Report += "<BR>\n";

	Report += GETTEXT("Your fleet(s) started detecting enemy fleets.");

	Report += "<BR>\n";



	CDefensePlan

		DefensePlan;

	DefensePlan.init_for_empire(DefenderFleetList, false);



	if (Battle.init_battle_fleet(&OffensePlan, AttackerFleetList, AttackerAdmiralList, &DefensePlan, DefenderFleetList, DefenderAdmiralList))

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



			Magistrate->remove_planet(TargetPlanet->get_id());



			aPlayer->add_planet(TargetPlanet);

			TargetPlanet->start_terraforming();



			TargetPlanet->type(QUERY_UPDATE);

			STORE_CENTER->store(*TargetPlanet);



			CEmpirePlanetInfoList *

			EmpirePlanetInfoList = EMPIRE->get_empire_planet_info_list();

			CEmpirePlanetInfo *

			EmpirePlanetInfo = EmpirePlanetInfoList->get_by_planet_id(TargetPlanet->get_id());

			if(EmpirePlanetInfo) {

				EmpirePlanetInfo->set_owner_id(aPlayer->get_game_id());



				EmpirePlanetInfo->type(QUERY_UPDATE);

				STORE_CENTER->store(*EmpirePlanetInfo);

			} else {

				SLOG("Planet id wasn't found id:%d",TargetPlanet->get_id());

			}



			Report.format(GETTEXT("You have gained new planet %1$s."), 

				TargetPlanet->get_name());

			Report += "<BR>\n";



			aPlayer->change_empire_relation(-50);



			ADMIN_TOOL->add_invade_magistrate_log(

				(char *)format("The player %s has succeeded in invasion against the magistrate(%d)",

					aPlayer->get_nick(),

					Magistrate->get_cluster_id()));

		}

		else

		{

			//Report += GETTEXT("Your fleet had critical damage in the fleet battle.");

			//Report += "<BR>\n";

			//Report += GETTEXT("Your fleet has been destroyed completely.");



			Report.format(GETTEXT("You have failed to invade %1$s."), EMPIRE->get_name());



			Buf.format(GETTEXT("%1$s invaded %2$s and lost."),

				aPlayer->get_nick(), EMPIRE->get_name());

			Buf += "<BR>\n";



			aPlayer->time_news((char *)Buf);



			ADMIN_TOOL->add_invade_magistrate_log(

				(char *)format("The player %s has failed in invasion against the magistrate(%d)",

					aPlayer->get_nick(),

					Magistrate->get_cluster_id()));

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



	CDefenseFleetList

	*OffensePlanFleetList = OffensePlan.get_fleet_list(),

	*DefensePlanFleetList = DefensePlan.get_fleet_list();

	OffensePlanFleetList->remove_all();

	DefensePlanFleetList->remove_all();



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

			if (!aPlayer->has_cluster(TargetPlanet->get_cluster_id()))

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



	return output("empire/invade_magistrate_result.html");

}

