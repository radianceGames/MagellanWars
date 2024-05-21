#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../battle.h"
#include "../../define.h"
#include <cstdio>

bool
CPageBlockadeResult::handler(CPlayer *aPlayer)
{
	system_log( "start page handler %s", get_name() );
	
	SLOG("Player %d attempted to initiate a blockade attack.", aPlayer->get_game_id());

	if (aPlayer->has_siege_blockade_restriction() == true)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Due to Imperial regulations, you may not siege for another %1$d seconds."), aPlayer->get_siege_blockade_restriction_time()));

		return output("war/war_error.html");
	}

	QUERY("MODE", ModeString);
	if (!ModeString) {
		ITEM("RESULT_MESSAGE",
			GETTEXT("You decided not to attack the enemy.<BR>"
				"Your fleets are returning home without attacking the planet.<BR>"
				"After some hours, your fleets will be on stand-by again."));
		return output("war/blockade_planet_return.html");
	}
	if (!strcasecmp("CANCEL", ModeString))
	{
		ITEM("RESULT_MESSAGE",
			GETTEXT("You decided not to attack the enemy.<BR>"
				"Your fleets are returning home without attacking the planet.<BR>"
				"After some hours, your fleets will be on stand-by again."));
		return output("war/blockade_planet_return.html");
	}

	QUERY("PLANET_ID", PlanetIDString);
	int
		PlanetID = as_atoi(PlanetIDString);
	CHECK(!PlanetID,
			GETTEXT("The planet's ID was not valid."));
	CPlanet *
		Planet = PLANET_TABLE->get_by_id(PlanetID);
	CHECK(!Planet, GETTEXT("There is no such planet."));
	CPlayer *
		TargetPlayer = Planet->get_owner();
	CHECK(!TargetPlayer,
			(char *)format(
					GETTEXT("The planet with ID %1$s has no owner."
							" Please ask Archspace Development Team."),
							dec2unit(PlanetID)));

	CHECK(TargetPlayer->get_game_id() == EMPIRE_GAME_ID,
			GETTEXT("You can't attack the Empire in this menu."));

	CHECK(TargetPlayer->get_game_id() == aPlayer->get_game_id(),
			GETTEXT("You can't blockade yourself."));

	CHECK(TargetPlayer->is_dead(), GETTEXT("Target player is dead."));

	if (TargetPlayer->has_siege_blockade_protection() == true)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Recently that player has had a planetary siege battle in his/her domain. You decided not to move your armada there until you get a clear information."));

		return output("war/war_error.html");
	}


	CString Attack;
	Attack = aPlayer->check_attackable(TargetPlayer);
	CHECK(Attack.length(), Attack);

	CHECK(!TargetPlayer->is_border_area(PlanetID), GETTEXT("The target planet is not a border planet."));

	CRelation* Relation = NULL;

	//if(aPlayer->get_council() == TargetPlayer->get_council())
		Relation = aPlayer->get_relation(TargetPlayer);
	if(!Relation)
	{
		Relation = (CCouncilRelation*)
							TargetPlayer->get_council()->get_relation(
												aPlayer->get_council());
	}
	if(!Relation)
	{
		ITEM("ERROR_MESSAGE",
					(char *)GETTEXT("You have no relation with the target player."));

		return output("war/war_error.html");
	}

	if(Relation->get_relation() != CRelation::RELATION_WAR && Relation->get_relation() != CRelation::RELATION_TOTAL_WAR && Relation->get_relation() != CRelation::RELATION_HOSTILE && Relation->get_relation() != CRelation::RELATION_BOUNTY)
	{
		ITEM("ERROR_MESSAGE",
					(char *)GETTEXT("You have no relation with the target player."));

		return output("war/war_error.html");

	}
	if((CGame::get_game_time() - Relation->get_time()) < 600)
	{
		ITEM("ERROR_MESSAGE",
							(char *)format(GETTEXT("You have not had hostile relations with the target player long enough to attack. Please wait another %1$s seconds"),
							dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
		return output("war/war_error.html");
	}
	
	SLOG("Player %d passed blockade attack checks against player %d, initiating blockade.", aPlayer->get_game_id(), TargetPlayer->get_game_id());

	// new honor loss punishment system
	int	HonorLoss = 0;
	int	AttackRatio = (long long int)TargetPlayer->get_power()*100/(long long int)aPlayer->get_power();

	if (AttackRatio < 60) HonorLoss += 12;
	else if (AttackRatio < 70) HonorLoss += 9;
	else if (AttackRatio < 80) HonorLoss += 7;
	else if (AttackRatio < 90) HonorLoss += 3;
	else if (AttackRatio < 100) HonorLoss += 1;

	// modify the loss by the difference of honor divided by 6
	HonorLoss += (int)((TargetPlayer->get_honor()-aPlayer->get_honor())/50);
	if ((Relation != NULL)&&(Relation->get_relation() == CRelation::RELATION_BOUNTY))
	{
		//person has a bounty on him
	}
	else if (aPlayer->get_council() == TargetPlayer->get_council())
	{
		if (Relation)
		{
			switch(Relation->get_relation())
			{
				case CRelation::RELATION_HOSTILE:
				case CRelation::RELATION_WAR:
				case CRelation::RELATION_BOUNTY:
					HonorLoss = 0;
					break;
				case CRelation::RELATION_TRUCE:
				case CRelation::RELATION_NONE:
					HonorLoss += 2;
					break;
				case CRelation::RELATION_PEACE:
					HonorLoss += 4;
					break;
				case CRelation::RELATION_ALLY:
					HonorLoss += 6;
					break;
				default:
					break;
			}
		} else HonorLoss += 2;
	}
	else
	{
		CCouncilRelation *Relation = (CCouncilRelation*)
							TargetPlayer->get_council()->get_relation(
												aPlayer->get_council());
		if (Relation)
		{
			switch(Relation->get_relation())
			{
				case CRelation::RELATION_HOSTILE:
				case CRelation::RELATION_WAR:
					HonorLoss = 0;
					break;
				case CRelation::RELATION_TRUCE:
				case CRelation::RELATION_NONE:
					HonorLoss += 2;
					break;
				case CRelation::RELATION_PEACE:
					HonorLoss += 4;
					break;
				case CRelation::RELATION_ALLY:
					HonorLoss += 6;
					break;
				case CRelation::RELATION_SUBORDINARY:
					if (Relation->get_council1() == aPlayer->get_council())
						HonorLoss += 3;
					break;
				default:
					break;
			}
		}
	}

	if (HonorLoss > 0) aPlayer->change_honor_with_news(-HonorLoss);
	if (aPlayer->get_honor() == 0) {
        aPlayer->get_council()->change_honor(-5);
    }

	QUERY("FLEET_NUMBER", FleetNumberString)
	int FleetNumber = as_atoi(FleetNumberString);
	CHECK(!FleetNumber, GETTEXT("Wrong fleet number received."
								" Please ask Archspace Development Team."));

	if(FleetNumber < 1 || FleetNumber > 20) {
 		ITEM("ERROR_MESSAGE",(char *)format(GETTEXT("Wrong fleet number received. Please ask Archspace Development Team.")));
 		return output("war/war_error.html");
 		}

	CBattle Battle(CBattle::WAR_BLOCKADE, aPlayer, TargetPlayer, (void *)Planet);

	CFleetList
		*AttackerFleetList = aPlayer->get_fleet_list(),
		*DefenderFleetList = TargetPlayer->get_fleet_list();
	CAdmiralList
		*AttackerAdmiralList = aPlayer->get_admiral_list(),
		*DefenderAdmiralList = TargetPlayer->get_admiral_list();

	CDefensePlan
		OffensePlan;
	OffensePlan.set_owner(aPlayer->get_game_id());

	for (int i=1 ; i<=FleetNumber ; i++)
	{
		char Query[100];

		sprintf(Query, "FLEET%d_O", i);
		QUERY(Query, CommandString);

		sprintf(Query, "FLEET%d_X", i);
		QUERY(Query, LocationXString);

		sprintf(Query, "FLEET%d_Y", i);
		QUERY(Query, LocationYString);

		sprintf(Query, "FLEET%d_ID", i);
		QUERY(Query, FleetIDString);

		CHECK(!CommandString ||
				!LocationXString ||
				!LocationYString ||
				!FleetIDString,
				GETTEXT("1 or more fleets' deployment data was not found."));

		int
			Command = as_atoi(CommandString),
			LocationX = as_atoi(LocationXString),
			LocationY = as_atoi(LocationYString),
			FleetID = as_atoi(FleetIDString);

		CFleet *
			Fleet = (CFleet *)AttackerFleetList->get_by_id(FleetID);
		CHECK(!Fleet,
				(char *)format(
						GETTEXT("You don't have a fleet with ID %1$s."),
						dec2unit(FleetID)));

		CHECK(Command < CDefenseFleet::COMMAND_NORMAL ||
			Command >= CDefenseFleet::COMMAND_MAX,
					(char *)format(GETTEXT("The fleet %1$s has a wrong command."),
									Fleet->get_name()));

		CHECK(LocationX < 9 || LocationX > 609 ||
			LocationY < 226 || LocationY > 426,
			format(GETTEXT("%1$s has wrong location in the battle field."), Fleet->get_nick()));

		CMission &Mission = Fleet->get_mission();
		CHECK(Mission.get_mission() != CMission::MISSION_SORTIE,
				format(GETTEXT("%1$s did not make a sortie."), Fleet->get_nick()));

#define SHIP_SIZE_CRUISER	5
		if (Planet->get_cluster_id() != aPlayer->get_home_cluster_id())
		{
			if (Fleet->get_size() < SHIP_SIZE_CRUISER)
			{
				ITEM("ERROR_MESSAGE",
						(char *)format(GETTEXT("The ship class of %1$s is too small to voyage out to the cluster that the player %2$s is in."),
										Fleet->get_nick(),
										TargetPlayer->get_nick()));

				return output("war/war_error.html");
			}

		}
#undef SHIP_SIZE_CRUISER

		if (i == 1) OffensePlan.set_capital(FleetID);

		CDefenseFleet *
			OffenseFleet = new CDefenseFleet();
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
		return output("war/war_error.html");
	}

	CString Report;
	CString BattleView;

	Report.format(GETTEXT("Your fleet(s) arrived on %1$s's orbit."),
					Planet->get_name());
	Report += "<BR>\n";
	Report += GETTEXT("Your fleet(s) started detecting enemy fleets.");
	Report += "<BR>\n";

	CDefensePlanList *
		DefensePlanList = TargetPlayer->get_defense_plan_list();
	CDefensePlan *
		DefensePlan = DefensePlanList->get_optimal_plan(aPlayer->get_race(), aPlayer->get_power(), TargetPlayer->get_power(), CDefensePlan::TYPE_BLOCKADE);

    SLOG("Attempting to initiate blockade %d -> %d", aPlayer->get_game_id(), TargetPlayer->get_game_id());

	if (Battle.init_battle_fleet(&OffensePlan, AttackerFleetList, AttackerAdmiralList, DefensePlan, DefenderFleetList, DefenderAdmiralList))
	{
	    //SLOG("Blockade initialized! %d -> %d", aPlayer->get_game_id(), TargetPlayer->get_game_id());
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

        //SLOG("Finished running steps! %d -> %d", aPlayer->get_game_id(), TargetPlayer->get_game_id());

		if (Battle.attacker_win())
		{
			Report += GETTEXT("You won the fleet battle on the orbit.");
			Report += "<BR>\n";
			Report.format(GETTEXT("You blockaded %1$s of %2$s successfully."),
							Planet->get_name(), TargetPlayer->get_nick());
			Report += "<BR>\n";

			CString Buf;
			Buf.format(GETTEXT("%1$s blockaded %2$s of %3$s and won."),
						aPlayer->get_nick(), Planet->get_name(),
						TargetPlayer->get_nick());
			aPlayer->time_news((char*)Buf);
			TargetPlayer->time_news((char*)Buf);
			Planet->start_blockade();

			if (Relation != NULL && Relation->get_relation() == CRelation::RELATION_BOUNTY)
			{
				CBounty *theBounty = aPlayer->get_bounty_by_player_id(TargetPlayer->get_game_id());
				if (theBounty != NULL)//does the player have the bounty on the guy?
				{
					int changeInDefenderPower = 0;
					int OriginalDefenderPower = 0;

					CBattleFleetList DefenderBattleFleetList = Battle.get_defense_battle_fleet_list();

					for(int i=0; i<DefenderBattleFleetList.length(); i++)
					{
						CBattleFleet *
							Fleet = (CBattleFleet*)DefenderBattleFleetList.get(i);
						if (Fleet == NULL) continue;
						OriginalDefenderPower += Fleet->get_original_power();
						changeInDefenderPower -= Fleet->get_power();
					}
					changeInDefenderPower += OriginalDefenderPower;
					if (OriginalDefenderPower < 1) OriginalDefenderPower = 1;
					if (changeInDefenderPower < 0) changeInDefenderPower = 0;

					Report += "The empire was pleased with your attack";
					Report += "<BR>\n";
					int EmpirePoints = number(changeInDefenderPower)*10/OriginalDefenderPower;
					if (EmpirePoints < 0) EmpirePoints = 0;
					if (EmpirePoints > 7) EmpirePoints = 7;
					theBounty->change_empire_points(EmpirePoints);
				}
			}
			ADMIN_TOOL->add_blockade_log(
					(char *)format("The player %s has succeeded to blockade %s whose owner is the player %s.",
									aPlayer->get_nick(),
									Planet->get_name(),
									TargetPlayer->get_nick()));
		}
		else
		{
		    //SLOG("Blockade initialized with no fleets! %d -> %d", aPlayer->get_game_id(), TargetPlayer->get_game_id());
			//Report += GETTEXT("Your fleet has been destroyed completely.");
			//Report += "<BR>\n";
			Report.format(GETTEXT("You have failed to blockade %1$s of %2$s."),
						Planet->get_name(), TargetPlayer->get_nick());
			Report += "<BR>\n";
			CString
				Buf;
			Buf.format(GETTEXT("%1$s blockaded %2$s of %3$s and lost."),
						aPlayer->get_nick(), Planet->get_name(),
						TargetPlayer->get_nick());
			aPlayer->time_news((char *)Buf);
			TargetPlayer->time_news((char *)Buf);

			ADMIN_TOOL->add_blockade_log(
					(char *)format("The player %s has failed to blockade %s whose owner is the player %s.",
									aPlayer->get_nick(),
									Planet->get_name(),
									TargetPlayer->get_nick()));
		}

		Battle.finish_report_after_battle();
		if (!CBattle::mBattleSimulationEnabled)
		{
			Battle.update_fleet_after_battle();
		}
		else
		{
			/*QUERY("SIMULATION", BattleSimulation);
			if (BattleSimulation != 1)
			{
				Battle.update_fleet_after_battle();
			}*/
		}
		Battle.save();

		BattleView.format("&nbsp;\n"
				"<A HREF=\"battle_report_detail.as?LOG_ID=%d\">"
					"<IMG SRC=\"%s/image/as_game/bu_view.gif\""
					"WIDTH=44 HEIGHT=11 BORDER=\"0\"></A>",
						Battle.get_record()->get_id(),
						(char *)CGame::mImageServerURL);
	}
	else
	{
		Planet->start_blockade();
		BattleView = GETTEXT("You had no fleet battle.");
		Report += GETTEXT("You encounter no defensive fleets.");
		Report += "<BR>\n";
		Report.format(GETTEXT("You blockaded %1$s of %2$s successfully."),
					Planet->get_name(), TargetPlayer->get_nick());
		Report += "<BR>\n";
		CString Buf;
		Buf.format(GETTEXT("%1$s blockaded %2$s of %3$s and won."),
					aPlayer->get_nick(), Planet->get_name(),
					TargetPlayer->get_nick());
		aPlayer->time_news((char *)Buf);
		TargetPlayer->time_news((char *)Buf);

		ADMIN_TOOL->add_blockade_log(
				(char *)format("The player %s has succeeded to blockade %s whose owner is the player %s.",
								aPlayer->get_nick(),
								Planet->get_name(),
								TargetPlayer->get_nick()));
	}

	CDefenseFleetList *
		OffensePlanFleetList = OffensePlan.get_fleet_list();
	OffensePlanFleetList->remove_all();
#if defined(PROTECTION_BEGINNER) || defined(PROTECTION_24HOURS)
	if(aPlayer->get_protected_mode() != CPlayer::PROTECTED_NONE)
	{
		aPlayer->set_protected_mode(CPlayer::PROTECTED_NONE);
	}
#endif

    //SLOG("Battle completed %d -> %d, setting return times", aPlayer->get_game_id(), TargetPlayer->get_game_id());

	for (int i=1 ; i<=FleetNumber ; i++)
	{
		char Query[100];

		sprintf(Query, "FLEET%d_ID", i); QUERY(Query, FleetIDString);

		int
			FleetID = as_atoi(FleetIDString);

		CFleet *
			Fleet = (CFleet *)AttackerFleetList->get_by_id(FleetID);

		if (Fleet)
		{
			CEngine *
				Engine = (CEngine *)COMPONENT_TABLE->get_by_id(Fleet->get_engine());
			assert(Engine);
			int ReturnTime = (9-Engine->get_level())*CGame::mSecondPerTurn;

			if (!aPlayer->has_cluster(Planet->get_cluster_id()))
				ReturnTime *= 2;

			if (aPlayer->has_ability(ABILITY_FAST_MANEUVER))
			{
				ReturnTime = ReturnTime * 70 / 100;
			}

			ReturnTime = aPlayer->calc_PA(ReturnTime, CPlayerEffect::PA_CHANGE_YOUR_FLEET_RETURN_TIME);
			ReturnTime = ReturnTime * CGame::mCombatReturnRate / 100;
			Fleet->init_mission(CMission::MISSION_RETURNING, 0);
			CMission &Mission = Fleet->get_mission();
			Mission.set_terminate_time(CGame::get_game_time() + ReturnTime);
		}
	}

	ITEM("BATTLE_VIEW", BattleView);
	ITEM("MESSAGE", Report);

	aPlayer->type(QUERY_UPDATE);
	TargetPlayer->type(QUERY_UPDATE);

	*STORE_CENTER << *aPlayer;
	*STORE_CENTER << *TargetPlayer;

	system_log( "end page handler %s", get_name() );
	if (aPlayer->is_protected())
	{
	   aPlayer->clear_lost_planet();
	}

	return output("war/blockade_result.html");
}

