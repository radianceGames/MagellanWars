#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../battle.h"
#include "../../define.h"
#include "../../player.h"
#include <cstdio>

bool
CPageSiegePlanetResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

    SLOG("Player %d attempted to initiate a siege attack.", aPlayer->get_game_id());

	if (aPlayer->has_siege_blockade_restriction() == true)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Due to Imperial regulations, you may not siege for another %1$d seconds."), aPlayer->get_siege_blockade_restriction_time()));

		return output("war/war_error.html");
	}

	QUERY("MODE", ModeString);
	if (!ModeString)
	{
		ITEM("RESULT_MESSAGE",
			GETTEXT("You decided not to attack the enemy.<BR>"
					"Your fleets are returning home without attacking the planet.<BR>"
					"After some hours, your fleets will be on stand-by again."));

		return output("war/siege_planet_planet_return.html");
	}

	if (!strcasecmp("CANCEL", ModeString))
	{
		ITEM("RESULT_MESSAGE",
			GETTEXT("You decided not to attack the enemy.<BR>"
					"Your fleets are returning home without attacking the planet.<BR>"
					"After some hours, your fleets will be on stand-by again."));

		return output("war/siege_planet_planet_return.html");
	}

	QUERY("PLANET_ID", PlanetIDString);
	int
		PlanetID = as_atoi(PlanetIDString);
	CHECK((PlanetID<=0),
			GETTEXT("The planet's ID was not valid."));

	CPlanet *
		Planet = PLANET_TABLE->get_by_id(PlanetID);
	CHECK(!Planet,
			(char *)format(
					GETTEXT("There is no planet with ID %1$s."),
					dec2unit(PlanetID)));

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
			GETTEXT("You can't attack yourself."));

	CHECK(TargetPlayer->is_dead(),
				format(GETTEXT("%1$s is dead."), TargetPlayer->get_nick()));

	if (TargetPlayer->has_siege_blockade_protection() == true)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("Recently that player has had a planetary siege battle in his/her domain. You decided not to move your armada there until you get a clear information."));

		return output("war/war_error.html");
	}

	CString Attack;
	Attack = aPlayer->check_attackable(TargetPlayer);
	CHECK(Attack.length(), Attack);

	CHECK(!TargetPlayer->is_border_area(PlanetID),
			(char *)format(
					GETTEXT("The planet %1$s is not a border planet."),
					Planet->get_name()));

	CCouncil *
		TargetPlayerCouncil = TargetPlayer->get_council();
	CRelation* Relation = aPlayer->get_relation(TargetPlayer);

	if (Relation != NULL && Relation->get_relation() == CRelation::RELATION_BOUNTY)
	{
	}
	else if (aPlayer->get_council() == TargetPlayerCouncil)
	{
		CHECK(!Relation,
			(char*)format(GETTEXT("You have no relation with %1$s."),
								TargetPlayer->get_nick()));

			CHECK((Relation->get_relation() != CRelation::RELATION_WAR && Relation->get_relation() != CRelation::RELATION_TOTAL_WAR),
			(char*)format(GETTEXT("You are not at war with %1$s."),
						TargetPlayer->get_nick()));
		if (TargetPlayer->is_protected())
		{
			ITEM("ERROR_MESSAGE",
			(char *)format(GETTEXT("The player %1$s has lost a large amount of their planets recently and Imperial Law prevents you from attacking them."),
			TargetPlayer->get_nick()));

			return output("war/war_error.html");
		}
		else if((CGame::get_game_time() - Relation->get_time()) < 600)
						{
							ITEM("ERROR_MESSAGE",
												(char *)format(GETTEXT("You have not had hostile relations with the target player long enough to attack. Please wait another %1$s seconds"),
							dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
			return output("war/war_error.html");
			}
	}
	else
	{
		Relation = TargetPlayerCouncil->get_relation(aPlayer->get_council());

		CHECK(!Relation,
			(char *)format(GETTEXT("Your council has no relationship with %1$s's council."),
							TargetPlayer->get_nick()));

		CHECK(Relation->get_relation() != CRelation::RELATION_WAR &&
				Relation->get_relation() != CRelation::RELATION_TOTAL_WAR,
			(char *)format(GETTEXT("Your council is not at war or total war with %1$s's council."),
								TargetPlayer->get_nick()));
		if (Relation->get_relation() == CRelation::RELATION_WAR)
		{
			if (TargetPlayer->is_protected())
			{
				ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("The player %1$s has lost a large amount of their planets recently and Imperial Law prevents you from attacking them."),
				TargetPlayer->get_nick()));
				return output("war/war_error.html");
			}
			if((CGame::get_game_time() - Relation->get_time()) < 600)
			{
				ITEM("ERROR_MESSAGE",
									(char *)format(GETTEXT("You have not had hostile relations with the target player long enough to attack. Please wait another %1$s seconds"),
							dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
				return output("war/war_error.html");
			}
		}
		else
		{
			if((CGame::get_game_time() - Relation->get_time()) < 600)
			{
				ITEM("ERROR_MESSAGE",
									(char *)format(GETTEXT("You have not had hostile relations with the target player long enough to attack. Please wait another %1$s seconds"),
							dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
				return output("war/war_error.html");
			}
		}
	}

	QUERY("FLEET_NUMBER", FleetNumberString)
	int FleetNumber = as_atoi(FleetNumberString);
	CHECK(!FleetNumber, GETTEXT("Wrong fleet number received."
								" Please ask Archspace Development Team."));

	if(FleetNumber < 1 || FleetNumber > 20)
	{
 		ITEM("ERROR_MESSAGE",(char *)format(GETTEXT("Wrong fleet number received. Please ask Archspace Development Team.")));
 		return output("war/war_error.html");
 	}


    SLOG("Player %d passed siege attack checks against player %d, initiating siege.", aPlayer->get_game_id(), TargetPlayer->get_game_id());

    bool PlanetLost = false;

	CBattle
		Battle(CBattle::WAR_SIEGE, aPlayer, TargetPlayer, (void *)Planet);

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
				GETTEXT("1 or more fleets' deployment data were not found."));

		int
			Command = as_atoi(CommandString),
			LocationX = as_atoi(LocationXString),
			LocationY = as_atoi(LocationYString),
			FleetID = as_atoi(FleetIDString);

		/*SLOG("Command:%d, LocationX:%d, LocationY:%d, FleetID:%d",
				Command, LocationX, LocationY, FleetID);*/

		CFleet *
			Fleet = (CFleet *)AttackerFleetList->get_by_id(FleetID);

		CHECK(!Fleet,
				(char *)format(
						GETTEXT("You don't have the fleet with ID %1$s."),
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

		if (i==1) OffensePlan.set_capital(FleetID);

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

    SLOG("Offense plan set %d -> %d ", aPlayer->get_game_id(), TargetPlayer->get_game_id());

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
		DefensePlan = DefensePlanList->get_optimal_plan(aPlayer->get_race(), aPlayer->get_power(), TargetPlayer->get_power(), CDefensePlan::TYPE_SIEGE);

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
			Report += GETTEXT("You have a counterattack from ground.");
			Report += "<BR>\n";
			if (Battle.siege_war())
			{
				Report.format(GETTEXT("You put down the military base(s) on %1$s."),
							Planet->get_name());
				Report += "<BR>\n";

				Buf.format(GETTEXT("%1$s invaded %2$s of %3$s and won."),
						aPlayer->get_nick(), Planet->get_name(),
						TargetPlayer->get_nick());
				Buf += "<BR>\n";

				aPlayer->time_news((char*)Buf);
				TargetPlayer->time_news((char*)Buf);
				TargetPlayer->drop_planet(Planet);
				TargetPlayer->lost_planet();
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

						if (number((int)((changeInDefenderPower)*100/OriginalDefenderPower) > 10))
						{
							aPlayer->add_planet(Planet);
							theBounty->change_empire_points(1);
							Report.format(GETTEXT("The empire was pleased and let you recieve planet %1$s."),
									Planet->get_name());
							Report += "<BR>\n";
						}
						else
						{
							Report.format(GETTEXT("The empire was pleased with you chivalry and added planet %1$s to is own domain."),
									Planet->get_name());
							Report += "<BR>\n";
							theBounty->change_empire_points(5);
							CMagistrateList *
								MagistrateList = EMPIRE->get_magistrate_list();
							CMagistrate *Magistrate;
							for (int i=0 ; i<MagistrateList->length() ; i++)
							{
								Magistrate = (CMagistrate *)MagistrateList->get(i);
								if (aPlayer->get_home_cluster_id() == Magistrate->get_cluster_id()) break;
							}
							if (Magistrate->is_dead())
							{
								Planet->set_owner(EMPIRE);
								EMPIRE->add_empire_planet(Planet);
							}
							else
							{
								Planet->set_owner(EMPIRE);
								Magistrate->add_planet(Planet);
							}
						}
					}
				}
				else
				{
					aPlayer->add_planet(Planet);
					Report.format(GETTEXT("You have gained new planet %1$s."),
								Planet->get_name());
					Report += "<BR>\n";
				}
				CEmpirePlanetInfo *
					EmpirePlanetInfo = EMPIRE_PLANET_INFO_LIST->get_by_planet_id(Planet->get_id());
				if (EmpirePlanetInfo != NULL)
				{
					EmpirePlanetInfo->set_owner_id(aPlayer->get_game_id());

					EmpirePlanetInfo->type(QUERY_UPDATE);
					STORE_CENTER->store(*EmpirePlanetInfo);
				}

				if (aPlayer->get_race() == TargetPlayer->get_race())
				{
					int Ratio = 10 + number(40)-1;
					int Ret = Planet->get_population() * Ratio / 100;
					Planet->change_population(-Ret);

					Report.format(GETTEXT("You exiled %1$s people from %2$s."),
									dec2unit(Ret),
									Planet->get_name());
					Ratio = 25 + number(50)-1;
					Ret = Planet->get_building().get(BUILDING_FACTORY) * Ratio / 100;
					Planet->get_building().change(BUILDING_FACTORY, -Ret);
					Report.format(GETTEXT("You destroyed %1$s unit(s) of factory."), dec2unit(Ret));
					Report += "<BR>\n";

					Ratio = 25 + number(50)-1;
					Ret = Planet->get_building().get(BUILDING_RESEARCH_LAB) * Ratio / 100;
					Planet->get_building().change(BUILDING_RESEARCH_LAB, -Ret);
					Report.format(GETTEXT("You destroyed %1$s unit(s) of research lab."), dec2unit(Ret));
					Report += "<BR>\n";

					Ratio = 25 + number(50)-1;
					Ret = Planet->get_building().get(BUILDING_MILITARY_BASE) * Ratio / 100;
					Planet->get_building().change(BUILDING_MILITARY_BASE, -Ret);
					Report.format(GETTEXT("You destroyed %1$s unit(s) of military base."),
									dec2unit(Ret));
					Report += "<BR>\n";
				}
				else
				{
					Planet->change_population(-Planet->get_population());
					Report.format(GETTEXT("You have a genocide on %1$s."),
									Planet->get_name());
					Report += "<BR>\n";

					Planet->change_population(10000);
					Report += GETTEXT("You transfer 10 mil. people.");
					Report += "<BR>\n";

					int Ratio = 50 + number(50)-1;
					int Ret = Planet->get_building().get(BUILDING_FACTORY) * Ratio / 100;
					Planet->get_building().change(BUILDING_FACTORY, -Ret);
					Report.format(GETTEXT("You destroyed %1$s unit(s) of factory."), dec2unit(Ret));
					Report += "<BR>\n";

					Ratio = 50 + number(50)-1;
					Ret = Planet->get_building().get(BUILDING_RESEARCH_LAB) * Ratio / 100;
					Planet->get_building().change(BUILDING_RESEARCH_LAB, -Ret);
					Report.format(GETTEXT("You destroyed %1$s unit(s) of research lab."), dec2unit(Ret));
					Report += "<BR>\n";

					Ratio = 50 + number(50)-1;
					Ret = Planet->get_building().get(BUILDING_MILITARY_BASE) * Ratio / 100;
					Planet->get_building().change(BUILDING_MILITARY_BASE, -Ret);
					Report.format(GETTEXT("You destroyed %1$s unit(s) of military base."), dec2unit(Ret));
					Report += "<BR>\n";
				}

				Planet->start_terraforming();

				if (TargetPlayer->get_planet_list()->length() == 0)
				{
					static CString
						TargetNick;
					TargetNick.clear();
					TargetNick = TargetPlayer->get_nick();
					TargetPlayer->set_dead(
							(char *)format("Player %s has killed %s with siege. [%s]",
											aPlayer->get_nick(),
											(char *)TargetNick,
											aPlayer->get_last_login_ip()));
					Report.format(GETTEXT("Your attack has brought about the ruin of %1$s."),
									TargetPlayer->get_nick());
					Report += "<BR>\n";
				}

				ADMIN_TOOL->add_siege_planet_log(
						(char *)format("The player %s has succeeded to siege the planet %s whose owner is the player %s.",
										aPlayer->get_nick(),
										Planet->get_name(),
										TargetPlayer->get_nick()));

/*				if (aPlayer->has_ability(ABILITY_TEMPORAL_MASTERY)) {
                    if (number(2) == 1) {
                        Report.format(GETTEXT("Temporal fluctuations causes the planet %1$s to disappear from existence."), Planet->get_name());
					    Report += "<BR>\n";

                        aPlayer->drop_planet(Planet);
                        aPlayer->lost_planet();
			            Planet->type(QUERY_DELETE);
			            STORE_CENTER->store(*Planet);
			            
			            PlanetLost = true;
                    }
                    if (number(4) == 1) {
                        int newClusterID = aPlayer->find_new_planet();
                        CCluster *Cluster = UNIVERSE->get_by_id(newClusterID);
				        assert(Cluster);
				        CPlanet *NewPlanet = new CPlanet();
				        NewPlanet->initialize();
				        NewPlanet->set_cluster(Cluster);
				        NewPlanet->set_name(Cluster->get_new_planet_name());
				        aPlayer->add_planet(NewPlanet);
				        PLANET_TABLE->add_planet(NewPlanet);
				        NewPlanet->start_terraforming();

				        STORE_CENTER->store(*NewPlanet);

				        Report.format(GETTEXT("A new planet %1$s has materialized from the time rifts."), NewPlanet->get_name());
					    Report += "<BR>\n";
                    }
                }*/
			}
			else
			{
				Report += GETTEXT("Your fleet had critical damage from the ground counterattack.");
				Report += "<BR>\n";
				Report += GETTEXT("Your fleet has been destroyed completely.");

				Buf.format(GETTEXT("%1$s invaded %2$s of %3$s and lost."),
						aPlayer->get_nick(), Planet->get_name(),
						TargetPlayer->get_nick());

				aPlayer->time_news((char*)Buf);
				TargetPlayer->time_news((char*)Buf);


				ADMIN_TOOL->add_siege_planet_log(
						(char *)format("The player %s has failed to siege the planet %s whose owner is the player %s.",
										aPlayer->get_nick(),
										Planet->get_name(),
										TargetPlayer->get_nick()));
			}


		}
		else
		{
			//Report += GETTEXT("Your fleet had critical damage in the fleet battle.");
			//Report += "<BR>\n";
			//Report += GETTEXT("Your fleet has been destroyed completely.");

			Report.format(GETTEXT("You have failed to invade %1$s of %2$s."), Planet->get_name(), TargetPlayer->get_nick());

			Buf.format(GETTEXT("%1$s invaded %2$s of %3$s and lost."),
					aPlayer->get_nick(), Planet->get_name(),
					TargetPlayer->get_nick());

			aPlayer->time_news((char*)Buf);
			TargetPlayer->time_news((char*)Buf);


			ADMIN_TOOL->add_siege_planet_log(
					(char *)format("The player %s has failed to siege the planet %s whose owner is the player %s.",
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
	else // have no defensive fleet
	{
		BattleView = GETTEXT("You had no fleet battle.");
		Report += GETTEXT("You encounter no defensive fleets.");
		Report += "<BR>\n";
		Report += GETTEXT("You have a counterattack from ground.");
		Report += "<BR>\n";
		CString Buf;
		if (Battle.siege_war())
		{
			Report.format(GETTEXT("You put down the military base(s) on %1$s."),
							Planet->get_name());
			Report += "<BR>\n";

			Buf.format(GETTEXT("%1$s invaded %2$s of %3$s and won."),
					aPlayer->get_nick(), Planet->get_name(),
						TargetPlayer->get_nick());

			aPlayer->time_news((char*)Buf);
			TargetPlayer->time_news((char*)Buf);

			TargetPlayer->drop_planet(Planet);
			TargetPlayer->lost_planet();

			if (Relation != NULL && Relation->get_relation() == CRelation::RELATION_BOUNTY)
			{
				CBounty *theBounty = aPlayer->get_bounty_by_player_id(TargetPlayer->get_game_id());
				if (theBounty != NULL)
				{
					if (number(10)>7)
					{
						aPlayer->add_planet(Planet);
						theBounty->change_empire_points(1);
						Report.format(GETTEXT("The empire was pleased and let you recieve planet %1$s."),
								Planet->get_name());
						Report += "<BR>\n";

						CEmpirePlanetInfo *
							EmpirePlanetInfo = EMPIRE_PLANET_INFO_LIST->get_by_planet_id(Planet->get_id());
						if (EmpirePlanetInfo != NULL)
						{
							EmpirePlanetInfo->set_owner_id(aPlayer->get_game_id());

							EmpirePlanetInfo->type(QUERY_UPDATE);
							STORE_CENTER->store(*EmpirePlanetInfo);
						}
					}
					else
					{
						Report.format(GETTEXT("The empire was pleased with you chivalry and added planet %1$s to its own domain."),
								Planet->get_name());
						Report += "<BR>\n";
		       				theBounty->change_empire_points(4);
						CMagistrateList *
							MagistrateList = EMPIRE->get_magistrate_list();

						CMagistrate *Magistrate;
						for (int i=0 ; i<MagistrateList->length() ; i++)
						{
							Magistrate = (CMagistrate *)MagistrateList->get(i);
							if (Planet->get_cluster_id() == Magistrate->get_cluster_id())
							{
								CEmpirePlanetInfo *
									EmpirePlanetInfo = EMPIRE_PLANET_INFO_LIST->get_by_planet_id(Planet->get_id());

								if (EmpirePlanetInfo != NULL)
								{
									EmpirePlanetInfo->set_owner_id(EMPIRE_GAME_ID);

									EmpirePlanetInfo->type(QUERY_UPDATE);
								}
								else
								{
									EmpirePlanetInfo = new CEmpirePlanetInfo();
									EmpirePlanetInfo->set_planet_id(Planet->get_id());
									EmpirePlanetInfo->set_owner_id(EMPIRE_GAME_ID);

									EmpirePlanetInfo->type(QUERY_INSERT);

									EMPIRE_PLANET_INFO_LIST->add_empire_planet_info(EmpirePlanetInfo);
								}

								if (Magistrate->is_dead())
								{
									Planet->set_owner(EMPIRE);
									EMPIRE->add_empire_planet(Planet);
									EmpirePlanetInfo->set_planet_type(CEmpire::LAYER_EMPIRE_PLANET);
								}
								else
								{
									Planet->set_owner(EMPIRE);
									Magistrate->add_planet(Planet);
									EmpirePlanetInfo->set_planet_type(CEmpire::LAYER_MAGISTRATE);
								}
								STORE_CENTER->store(*EmpirePlanetInfo);
								break;
							}
						}
					}
				}
				else
				{
					aPlayer->add_planet(Planet);

					CEmpirePlanetInfo *
						EmpirePlanetInfo = EMPIRE_PLANET_INFO_LIST->get_by_planet_id(Planet->get_id());
					if (EmpirePlanetInfo != NULL)
					{
						EmpirePlanetInfo->set_owner_id(aPlayer->get_game_id());

						EmpirePlanetInfo->type(QUERY_UPDATE);
						STORE_CENTER->store(*EmpirePlanetInfo);
					}
				}
			}
			else
			{
				aPlayer->add_planet(Planet);

				CEmpirePlanetInfo *
					EmpirePlanetInfo = EMPIRE_PLANET_INFO_LIST->get_by_planet_id(Planet->get_id());

				if (EmpirePlanetInfo != NULL)
				{
					EmpirePlanetInfo->set_owner_id(aPlayer->get_game_id());

					EmpirePlanetInfo->type(QUERY_UPDATE);
					STORE_CENTER->store(*EmpirePlanetInfo);
				}
			}


			if (aPlayer->get_race() == TargetPlayer->get_race())
			{
				int Ratio = 10 + number(40)-1;
				int Ret = Planet->get_population() * Ratio / 100;
				Planet->change_population(-Ret);
				Report.format(GETTEXT("You exiled %1$s people from %2$s."),
								dec2unit(Ret),
								Planet->get_name());
				Report += "<BR>\n";

				Ratio = 25 + number(50)-1;
				Ret = Planet->get_building().get(BUILDING_FACTORY) * Ratio / 100;
				Planet->get_building().change(BUILDING_FACTORY, -Ret);
				Report.format(GETTEXT("You destroyed %1$s unit(s) of factory."), dec2unit(Ret));
				Report += "<BR>\n";

				Ratio = 25 + number(50)-1;
				Ret = Planet->get_building().get(BUILDING_RESEARCH_LAB) * Ratio / 100;
				Planet->get_building().change(BUILDING_RESEARCH_LAB, -Ret);
				Report.format(GETTEXT("You destroyed %1$s unit(s) of research lab."), dec2unit(Ret));
				Report += "<BR>\n";

				Ratio = 25 + number(50)-1;
				Ret = Planet->get_building().get(BUILDING_MILITARY_BASE) * Ratio / 100;
				Planet->get_building().change(BUILDING_MILITARY_BASE, -Ret);
				Report.format(GETTEXT("You destroyed %1$s unit(s) of military base."), dec2unit(Ret));
				Report += "<BR>\n";

			}
			else
			{
				Planet->change_population(-Planet->get_population());
				Report.format(GETTEXT("You have a genocide on %1$s."),
								Planet->get_name());
				Report += "<BR>\n";
				Planet->change_population(10000);


				int Ratio = 50 + number(50)-1;
				int Ret = Planet->get_building().get(BUILDING_FACTORY) * Ratio / 100;
				Planet->get_building().change(BUILDING_FACTORY, -Ret);
				Report.format(GETTEXT("You destroyed %1$s unit(s) of factory."), dec2unit(Ret));
				Report += "<BR>\n";

				Ratio = 50 + number(50)-1;
				Ret = Planet->get_building().get(BUILDING_RESEARCH_LAB) * Ratio / 100;
				Planet->get_building().change(BUILDING_RESEARCH_LAB, -Ret);
				Report.format(GETTEXT("You destroyed %1$s unit(s) of research lab."), dec2unit(Ret));
				Report += "<BR>\n";

				Ratio = 50 + number(50)-1;
				Ret = Planet->get_building().get(BUILDING_MILITARY_BASE) * Ratio / 100;
				Planet->get_building().change(BUILDING_MILITARY_BASE, -Ret);
				Report.format(GETTEXT("You destroyed %1$s unit(s) of military base."), dec2unit(Ret));
				Report += "<BR>\n";
			}

			Planet->start_terraforming();

			if (TargetPlayer->get_planet_list()->length() == 0)
			{
				static CString
					TargetNick;
				TargetNick.clear();
				TargetNick = TargetPlayer->get_nick();

				TargetPlayer->set_dead(
						(char *)format("Player %s has killed %s with siege. [%s]",
										aPlayer->get_nick(),
										(char *)TargetNick,
										aPlayer->get_last_login_ip()));

				Report.format(GETTEXT("Your attack has brought about"
										" the ruin of %s."),
								TargetPlayer->get_nick());
				Report += "<BR>\n";
				if (Relation != NULL && Relation->get_relation() == CRelation::RELATION_BOUNTY)
				{
					CBounty *theBounty = aPlayer->get_bounty_by_player_id(TargetPlayer->get_game_id());
					if (theBounty != NULL)
					{
						theBounty->change_empire_points(10);
						Report.format(GETTEXT("Your destruction of %s has pleased the empire greatly"),TargetPlayer->get_nick());
					}
				}
			}

			if (CGame::mSiegeBlockadeRestriction) aPlayer->set_siege_blockade_restriction();
			if (CGame::mSiegeBlockadeProtection) TargetPlayer->set_siege_blockade_protection();

			//aPlayer->set_siege_blockade_restriction();
			//TargetPlayer->set_siege_blockade_protection();

			ADMIN_TOOL->add_siege_planet_log(
					(char *)format("The player %s has succeeded to siege the planet %s whose owner is the player %s.",
									aPlayer->get_nick(),
									Planet->get_name(),
									TargetPlayer->get_nick()));
			
			// Temporal Mastery
/*			if (aPlayer->has_ability(ABILITY_TEMPORAL_MASTERY)) {
                SLOG("I have Temporal Mastery!");
                    if (number(2) == 1) {
                        SLOG("I am going to delete the planet!");
                        Report.format(GETTEXT("Temporal fluctuations causes the planet %1$s to disappear from existence."), Planet->get_name());
					    Report += "<BR>\n";

                        aPlayer->drop_planet(Planet);
                        SLOG("Planet dropped!");
                        aPlayer->lost_planet();
                        SLOG("Planet lost!");
			            Planet->type(QUERY_DELETE);
			            SLOG("SQL Updated!");
			            STORE_CENTER->store(*Planet);
			            SLOG("Planet Stored!");

			            PlanetLost = true;
                    }
                    if (number(4) == 1) {
                        SLOG("I am going to find a new planet!");
                        int newClusterID = aPlayer->find_new_planet();
                        CCluster *Cluster = UNIVERSE->get_by_id(newClusterID);
                        SLOG("Cluster %d!", newClusterID);
				        assert(Cluster);
				        CPlanet *NewPlanet = new CPlanet();
				        SLOG("New planet!!!");
				        NewPlanet->initialize();
				        SLOG("New planet inited!!!");
				        NewPlanet->set_cluster(Cluster);
				        SLOG("Cluster set!!!");
				        NewPlanet->set_name(Cluster->get_new_planet_name());
				        SLOG("I have a name now!!!");
				        aPlayer->add_planet(NewPlanet);
				        SLOG("I am owned!!");
				        PLANET_TABLE->add_planet(NewPlanet);
				        SLOG("I am added!!");
				        NewPlanet->start_terraforming();
				        SLOG("I am terraforming!!");

				        STORE_CENTER->store(*NewPlanet);

				        Report.format(GETTEXT("A new planet %1$s has materialized from the time rifts."), NewPlanet->get_name());
					    Report += "<BR>\n";
                    }
                }     */
		}
		else
		{
			Buf.format(GETTEXT("%1$s invaded %2$s of %3$s and lost."),
					aPlayer->get_nick(), Planet->get_name(),
					TargetPlayer->get_nick());

			aPlayer->time_news((char*)Buf);
			TargetPlayer->time_news((char*)Buf);

			Report += GETTEXT("Your fleet had critical damage from the ground counterattack.");
			Report += "<BR>\n";
			Report += GETTEXT("Your fleet has been destroyed completely.");


			ADMIN_TOOL->add_siege_planet_log(
					(char *)format("The player %s has failed to siege the planet %s whose owner is the player %s.",
									aPlayer->get_nick(),
									Planet->get_name(),
									TargetPlayer->get_nick()));
		}
		Battle.finish_report_after_battle();
		Battle.update_fleet_after_battle();
		Battle.save();
	}

	ITEM("MESSAGE", Report);
	ITEM("BATTLE_VIEW", BattleView);
#if defined(PROTECTION_BEGINNER) || defined(PROTECTION_24HOURS)
	if(aPlayer->get_protected_mode() != CPlayer::PROTECTED_NONE)
	{
		aPlayer->set_protected_mode(CPlayer::PROTECTED_NONE);
	}
#endif

	CDefenseFleetList *
		OffensePlanFleetList = OffensePlan.get_fleet_list();
	OffensePlanFleetList->remove_all();

    SLOG("Battle completed %d -> %d, setting return times", aPlayer->get_game_id(), TargetPlayer->get_game_id());

	for (int i=1 ; i<=FleetNumber ; i++)
	{
		char Query[100];
		sprintf(Query, "FLEET%d_ID", i); QUERY(Query, FleetIDString);

		int FleetID = as_atoi(FleetIDString);

		CFleet *
			Fleet = (CFleet *)AttackerFleetList->get_by_id(FleetID);

		if (Fleet)
		{
			CEngine *Engine =
				(CEngine*)COMPONENT_TABLE->get_by_id(Fleet->get_engine());
			assert(Engine);
			int ReturnTime = (9-Engine->get_level())*CGame::mSecondPerTurn;

			if (!aPlayer->has_cluster(Planet->get_cluster_id())) ReturnTime *= 2;

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

	aPlayer->type(QUERY_UPDATE);
	TargetPlayer->type(QUERY_UPDATE);
    //Planet->type(QUERY_UPDATE);

	*STORE_CENTER << *aPlayer;
	*STORE_CENTER << *TargetPlayer;
	
    if (!PlanetLost) {
        Planet->type(QUERY_UPDATE);
        STORE_CENTER->store(*Planet);
    } else {
        delete(Planet);
    }

	if (aPlayer->is_protected())
	{
		aPlayer->clear_lost_planet();
	}

//	system_log( "end page handler %s", get_name() );

	return output("war/siege_planet_result.html");
}
