#include <libintl.h>
#include "../triggers.h"
#include "../archspace.h"
#include "../council.h"
#include "../battle.h"

#define MAGISTRATE_ATTACK_MAXIMUIM_RELATION	-50
#define MAGISTRATE_ATTACK_CHANCE		.30
#define MAGISTRATE_ATTACK_MINIMUIM_POWER	4000000
#define MAGISTRATE_ATTACK_MAXIMUIM_POWER	15000000
#define MAGISTRATE_ATTACK_MEDIAN_FLEET_POWER	8000000

#define EMPIRE_ATTACK_MAXIMUIM_RELATION		-90
#define EMPIRE_ATTACK_CHANCE			.10
#define EMPIRE_ATTACK_MINIMUIM_POWER		15000001
#define EMPIRE_ATTACK_MAXIMUIM_POWER		MAX_POWER
#define EMPIRE_ATTACK_MEDIAN_FLEET_POWER	20000000

void
CCronTabEmpireCounterattackMagistrate::handler()
{
#ifndef NOEMPIRERETAL
   	if (!CGame::mUpdateTurn) return;
    SLOG("EmpireCounterattackMagistrate Start");
	if (EMPIRE->is_dead() == false)
	{
		CMagistrateList *
			MagistrateList = EMPIRE->get_magistrate_list();
		for (int i=0 ; i<MagistrateList->length() ; i++)
		{
			CMagistrate *
				Magistrate = (CMagistrate *)MagistrateList->get(i);
			if (Magistrate->is_dead() == true) continue;

			int index = 0;

			while (index < PLAYER_TABLE->length())
			{
				CPlayer *TargetPlayer = (CPlayer*)PLAYER_TABLE->get(index);
				index++;
				if (!TargetPlayer) continue;
				if (TargetPlayer->is_dead()) continue;
				if (TargetPlayer->get_home_cluster_id() != Magistrate->get_cluster_id()) continue;
				if (TargetPlayer->get_empire_relation() > MAGISTRATE_ATTACK_MAXIMUIM_RELATION) continue;
				if (number(MAX_GENERAL_INT/4+ TargetPlayer->get_empire_relation() -MAGISTRATE_ATTACK_MAXIMUIM_RELATION) > MAX_GENERAL_INT/4*MAGISTRATE_ATTACK_CHANCE) continue;
				if ((TargetPlayer->get_power() < MAGISTRATE_ATTACK_MINIMUIM_POWER)||(TargetPlayer->get_power() > MAGISTRATE_ATTACK_MAXIMUIM_POWER)) continue;
				CPlanetList *
					aPlanetList = TargetPlayer->get_planet_list();

				if (aPlanetList->length() < 2) continue;
				CPlanet *
					TargetPlanet = (CPlanet*)aPlanetList->get(aPlanetList->length()-1);

				CBattle
					Battle(CBattle::WAR_MAGISTRATE_COUNTERATTACK, EMPIRE, TargetPlayer, (void *)TargetPlanet);

				CAdmiralList
					*AttackerAdmiralList = EMPIRE->get_admiral_list(),
					*DefenderAdmiralList = TargetPlayer->get_admiral_list();
				CFleetList
					*AttackerFleetList = EMPIRE->get_fleet_list(),
					*DefenderFleetList = TargetPlayer->get_fleet_list();

				AttackerAdmiralList->remove_all();
				AttackerFleetList->remove_all();
				int aFleetRatio = (int)(((long long int)TargetPlayer->get_power() - (long long int)aPlanetList->length()*1300)*100 / MAGISTRATE_ATTACK_MEDIAN_FLEET_POWER);
				if (aFleetRatio > 200) aFleetRatio = 200;
				if (aFleetRatio < 20) aFleetRatio = 20;
				AttackerFleetList->create_as_empire_fleet_group_volatile(AttackerAdmiralList, CEmpire::EMPIRE_FLEET_GROUP_TYPE_MAGISTRATE_COUNTERATTACK, Magistrate->get_cluster_id(), aFleetRatio);

				CDefensePlan
					OffensePlan;
				OffensePlan.init_for_empire(AttackerFleetList, true);

				CString Report;

				CDefensePlanList *
					DefensePlanList = TargetPlayer->get_defense_plan_list();
				CDefensePlan *
					DefensePlan = DefensePlanList->get_generic_plan();

				if (Battle.init_battle_fleet(&OffensePlan, AttackerFleetList, AttackerAdmiralList, DefensePlan, DefenderFleetList, DefenderAdmiralList))
				{
					while(Battle.run_step());

					if (Battle.attacker_win())
					{

						TargetPlayer->time_news(
							(char *)format(GETTEXT("%1$s invaded %2$s and won."),
										EMPIRE->get_name(), TargetPlayer->get_nick()));

						TargetPlayer->change_empire_relation(2);
						TargetPlayer->drop_planet(TargetPlanet);

						TargetPlanet->set_owner(EMPIRE);
						Magistrate->add_planet(TargetPlanet);

						CEmpirePlanetInfo *
							EmpirePlanetInfo = EMPIRE_PLANET_INFO_LIST->get_by_planet_id(TargetPlanet->get_id());
						if (EmpirePlanetInfo != NULL)
						{
							EmpirePlanetInfo->set_owner_id(EMPIRE_GAME_ID);

							EmpirePlanetInfo->type(QUERY_UPDATE);
							STORE_CENTER->store(*EmpirePlanetInfo);
						}
						else
						{
							EmpirePlanetInfo = new CEmpirePlanetInfo();
							EmpirePlanetInfo->set_planet_id(TargetPlanet->get_id());
							EmpirePlanetInfo->set_owner_id(EMPIRE_GAME_ID);
							EmpirePlanetInfo->set_planet_type(CEmpire::LAYER_MAGISTRATE);
							EmpirePlanetInfo->set_position_arg(Magistrate->get_cluster_id());

							EmpirePlanetInfo->type(QUERY_INSERT);
							STORE_CENTER->store(*EmpirePlanetInfo);

							EMPIRE_PLANET_INFO_LIST->add_empire_planet_info(EmpirePlanetInfo);
						}

						TargetPlanet->type(QUERY_UPDATE);
						STORE_CENTER->store(*TargetPlanet);
					}
					else
					{
						TargetPlayer->time_news(
							(char *)format(GETTEXT("%1$s invaded %2$s and lost."),
										EMPIRE->get_name(), TargetPlayer->get_nick()));
					}

					Battle.finish_report_after_battle();
					Battle.update_fleet_after_battle();
					Battle.save();
				}

				CDefenseFleetList *
					OffensePlanFleetList = OffensePlan.get_fleet_list();
				OffensePlanFleetList->remove_all();

				AttackerAdmiralList->remove_all();
				AttackerFleetList->remove_all();
			}
		}
	}
	SLOG("EmpireCounterattackMagistrate End");
#endif
}

void
CCronTabEmpireCounterattackEmpirePlanet::handler()
{
#ifndef NOEMPIRERETAL
   	if (!CGame::mUpdateTurn) return;
    SLOG("EmpireCounterattackEmpirePlanet Start");
	if (EMPIRE->is_dead() == false)
	{
		int index = 0;

		while (index < PLAYER_TABLE->length())
		{
			CPlayer *TargetPlayer = (CPlayer*)PLAYER_TABLE->get(index);
			index++;
			if (!TargetPlayer) continue;
			if (TargetPlayer->is_dead()) continue;
			if (TargetPlayer->get_empire_relation() > EMPIRE_ATTACK_MAXIMUIM_RELATION) continue;
			if (number(MAX_GENERAL_INT/4+ TargetPlayer->get_empire_relation() -EMPIRE_ATTACK_MAXIMUIM_RELATION) > MAX_GENERAL_INT/4*EMPIRE_ATTACK_CHANCE) continue;
			if ((TargetPlayer->get_power() < EMPIRE_ATTACK_MINIMUIM_POWER)||(TargetPlayer->get_power() > EMPIRE_ATTACK_MAXIMUIM_POWER)) continue;
			CPlanetList *
				aPlanetList = TargetPlayer->get_planet_list();

			if (aPlanetList->length() < 2) continue;
			CPlanet *
				TargetPlanet = (CPlanet*)aPlanetList->get(aPlanetList->length()-1);

			CBattle
				Battle(CBattle::WAR_EMPIRE_PLANET_COUNTERATTACK, EMPIRE, TargetPlayer, (void *)TargetPlanet);

			CAdmiralList
				*AttackerAdmiralList = EMPIRE->get_admiral_list(),
				*DefenderAdmiralList = TargetPlayer->get_admiral_list();
			CFleetList
				*AttackerFleetList = EMPIRE->get_fleet_list(),
				*DefenderFleetList = TargetPlayer->get_fleet_list();

			AttackerAdmiralList->remove_all();
			AttackerFleetList->remove_all();
			int aFleetRatio = (int)(((long long int)TargetPlayer->get_power() - (long long int)aPlanetList->length()*1300)*100 / EMPIRE_ATTACK_MEDIAN_FLEET_POWER);
			if (aFleetRatio > 200) aFleetRatio = 200;
			if (aFleetRatio < 20) aFleetRatio = 20;
			AttackerFleetList->create_as_empire_fleet_group_volatile(AttackerAdmiralList, CEmpire::EMPIRE_FLEET_GROUP_TYPE_EMPIRE_PLANET_COUNTERATTACK, -1, aFleetRatio);

			CDefensePlan
				OffensePlan;
			OffensePlan.init_for_empire(AttackerFleetList, true);

			CString Report;

			CDefensePlanList *
				DefensePlanList = TargetPlayer->get_defense_plan_list();
			CDefensePlan *
				DefensePlan = DefensePlanList->get_generic_plan();

			if (Battle.init_battle_fleet(&OffensePlan, AttackerFleetList, AttackerAdmiralList, DefensePlan, DefenderFleetList, DefenderAdmiralList))
			{
				while(Battle.run_step());

				if (Battle.attacker_win())
				{
					TargetPlayer->time_news(
						(char *)format(GETTEXT("%1$s invaded %2$s and won."),
									EMPIRE->get_name(), TargetPlayer->get_nick()));

					TargetPlayer->change_empire_relation(2);
					TargetPlayer->drop_planet(TargetPlanet);

					TargetPlanet->set_owner(EMPIRE);
					EMPIRE->add_empire_planet(TargetPlanet);

				CEmpirePlanetInfo *
					EmpirePlanetInfo = EMPIRE_PLANET_INFO_LIST->get_by_planet_id(TargetPlanet->get_id());
				if (EmpirePlanetInfo != NULL)
				{
					EmpirePlanetInfo->set_owner_id(EMPIRE_GAME_ID);

					EmpirePlanetInfo->type(QUERY_UPDATE);
					STORE_CENTER->store(*EmpirePlanetInfo);
				}
				else
				{
					EmpirePlanetInfo = new CEmpirePlanetInfo();
					EmpirePlanetInfo->set_planet_id(TargetPlanet->get_id());
					EmpirePlanetInfo->set_owner_id(EMPIRE_GAME_ID);
					EmpirePlanetInfo->set_planet_type(CEmpire::LAYER_EMPIRE_PLANET);

					EmpirePlanetInfo->type(QUERY_INSERT);
					STORE_CENTER->store(*EmpirePlanetInfo);

					EMPIRE_PLANET_INFO_LIST->add_empire_planet_info(EmpirePlanetInfo);
				}

					TargetPlanet->type(QUERY_UPDATE);
					STORE_CENTER->store(*TargetPlanet);
				}
				else
				{
					TargetPlayer->time_news(
						(char *)format(GETTEXT("%1$s invaded %2$s and lost."),
									EMPIRE->get_name(), TargetPlayer->get_nick()));
				}

				Battle.finish_report_after_battle();
				Battle.update_fleet_after_battle();
				Battle.save();
			}

			CDefenseFleetList *
				OffensePlanFleetList = OffensePlan.get_fleet_list();
			OffensePlanFleetList->remove_all();

			AttackerAdmiralList->remove_all();
			AttackerFleetList->remove_all();
		}
	}
	SLOG("EmpireCounterattackEmpirePlanet End");
#endif
}

