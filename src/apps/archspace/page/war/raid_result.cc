#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../battle.h"
#include "../../define.h"

bool
CPageRaidResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	QUERY("MODE", ModeString);
	if (!ModeString)
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("You decided not to attack the enemy.<BR>"
						"Your fleets are returning home without attacking the planet.<BR>"
						"After some hours, your fleets will be on stand-by again."));

		return output("war/raid_planet_return.html");
	}

	if (!strcasecmp("CANCEL", ModeString))
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("You decided not to attack the enemy.<BR>"
						"Your fleets are returning home without attacking the planet.<BR>"
						"After some hours, your fleets will be on stand-by again."));

		return output("war/raid_planet_return.html");
	}

	QUERY("TARGET_PLAYER_ID", TargetPlayerIDString);
	int
		TargetPlayerID = as_atoi(TargetPlayerIDString);
	CPlayer *
		TargetPlayer = PLAYER_TABLE->get_by_game_id(TargetPlayerID);
	CHECK(!TargetPlayer,
			(char *)format(GETTEXT("The player with ID %1$s doesn't exist."),
							dec2unit(TargetPlayerID)));

	CHECK(TargetPlayer->get_game_id() == EMPIRE_GAME_ID,
			GETTEXT("You can't attack the Empire in this menu."));

	CHECK(TargetPlayer->get_game_id() == aPlayer->get_game_id(),
			GETTEXT("You can't raid yourself."));

	CHECK(TargetPlayer->is_dead(),
			(char *)format(GETTEXT("%1$s is dead."),
							TargetPlayer->get_nick()));

	CString
		Attack;
	Attack = aPlayer->check_attackableHi(TargetPlayer);

	CHECK(Attack.length(), Attack);

	CCouncil *
		TargetPlayerCouncil = TargetPlayer->get_council();

	CRelation *
		Relation;
	CString
		RelationDescription;

        Relation = aPlayer->get_relation(TargetPlayer);

        if (aPlayer->get_council() == TargetPlayer->get_council() || (Relation != NULL && Relation->get_relation() == CRelation::RELATION_HOSTILE))
        {
		if (Relation)
			RelationDescription = Relation->get_relation_description();
		else
			RelationDescription = CRelation::get_relation_description(
								CRelation::RELATION_NONE);
	}
	else
	{
		Relation = TargetPlayerCouncil->get_relation(aPlayer->get_council());
		if (Relation)
		{
			RelationDescription.format("%s %s",
										GETTEXT("Council"),
										Relation->get_relation_description());
		}
		else
		{
			RelationDescription = CRelation::get_relation_description(
												CRelation::RELATION_NONE);
		}
	}
	if(!Relation)
	{
		ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("You have no relation with %1$s."),TargetPlayer->get_nick()));

		return output("war/war_error.html");
	}

	if(Relation->get_relation() != CRelation::RELATION_WAR && Relation->get_relation() != CRelation::RELATION_TOTAL_WAR && Relation->get_relation() != CRelation::RELATION_HOSTILE && Relation->get_relation() != CRelation::RELATION_BOUNTY)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You have no relation with %1$s."),TargetPlayer->get_nick()));

		return output("war/war_error.html");

	}
	if((CGame::get_game_time() - Relation->get_time()) < 600)
	{
		ITEM("ERROR_MESSAGE",
							(char *)format(GETTEXT("You have not had hostile relations with the target player long enough to attack. Please wait another %1$s seconds"),
							dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
		return output("war/war_error.html");
	}

	QUERY("RAID_FLEET_ID", RaidFleetIDString);
	int
		RaidFleetID = as_atoi(RaidFleetIDString);
	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	CFleet *
		RaidFleet = (CFleet *)FleetList->get_by_id(RaidFleetID);
	CHECK(!RaidFleet,
			GETTEXT("You did not select any fleet.<BR>"
					"Please try again."));
	CMission &
		Mission = RaidFleet->get_mission();
	CHECK(Mission.get_mission() != CMission::MISSION_SORTIE,
			format(GETTEXT("%1$s fleet didn't make a sortie or is on mission."),
					RaidFleet->get_nick()));

	QUERY("PLANET_ID", PlanetIDString);
	int
		PlanetID = as_atoi(PlanetIDString);
	CPlanet *
		Planet = TargetPlayer->get_planet_list()->get_by_id(PlanetID);
	CHECK(!Planet,
			(char *)format(GETTEXT("%1$s doesn't have a planet with ID %1$s."),
							TargetPlayer->get_nick(),
							dec2unit(PlanetID)));

#define SHIP_SIZE_CRUISER	5
		if (Planet->get_cluster_id() != aPlayer->get_home_cluster_id())
		{
			if (RaidFleet->get_size() < SHIP_SIZE_CRUISER)
			{
				ITEM("ERROR_MESSAGE",
						(char *)format(GETTEXT("The ship class of %1$s is too small to voyage out to the cluster that the player %2$s is in."),
										RaidFleet->get_nick(),
										TargetPlayer->get_nick()));

				return output("war/war_error.html");
			}

		}
#undef SHIP_SIZE_CRUISER

	CBattle
		Battle(CBattle::WAR_RAID, aPlayer, TargetPlayer, (void *)Planet);

	Battle.init_raid(RaidFleet);

	bool
		IsSuccessful = false;
	if (Battle.raid_fleet_detection_check())
	{
		if (Battle.raid_fleet_war())
		{
			if (Battle.raid_fleet_detection_check())
			{
				Battle.raid_bomb();
				IsSuccessful = true;

				Battle.raid_siege_war();
			}
			else
			{
				Battle.raid_bomb();
				IsSuccessful = true;
			}
		}
	}
	else
	{
		Battle.raid_bomb();
		IsSuccessful = true;
	}

	if (IsSuccessful == true)
	{
		ADMIN_TOOL->add_raid_log(
				(char *)format("The player %s has succeeded to raid the planet %s whose owner is %s.",
								aPlayer->get_nick(),
								Planet->get_name(),
								TargetPlayer->get_nick()));
	}
	else
	{
		ADMIN_TOOL->add_raid_log(
				(char *)format("The player %s has failed to raid the planet %s whose owner is %s.",
								aPlayer->get_nick(),
								Planet->get_name(),
								TargetPlayer->get_nick()));
	}

	Battle.update_fleet_after_battle();
#if defined(PROTECTION_BEGINNER) || defined(PROTECTION_24HOURS)
	if(aPlayer->get_protected_mode() != CPlayer::PROTECTED_NONE)
	{
		aPlayer->set_protected_mode(CPlayer::PROTECTED_NONE);
	}
#endif

	RaidFleet = (CFleet *)FleetList->get_by_id(RaidFleetID);
	if (RaidFleet)
	{
		CEngine *
			Engine = (CEngine *)COMPONENT_TABLE->get_by_id(RaidFleet->get_engine());
		assert(Engine);

		int
			ReturnTime = (9-Engine->get_level())*CGame::mSecondPerTurn;
		if (!aPlayer->has_cluster(Planet->get_cluster_id()))
			ReturnTime *= 2;
		if (aPlayer->has_ability(ABILITY_FAST_MANEUVER))
		{
			ReturnTime = ReturnTime * 70 / 100;
		}
		ReturnTime = aPlayer->calc_PA(ReturnTime, CPlayerEffect::PA_CHANGE_YOUR_FLEET_RETURN_TIME);
		ReturnTime = ReturnTime * CGame::mCombatReturnRate / 100;
		RaidFleet->init_mission(CMission::MISSION_RETURNING, 0);
		CMission &Mission = RaidFleet->get_mission();
		Mission.set_terminate_time(CGame::get_game_time() + ReturnTime);
	}

	ITEM("RESULT_MESSAGE", Battle.get_report());

	aPlayer->type(QUERY_UPDATE);
	*STORE_CENTER << *aPlayer;

	TargetPlayer->type(QUERY_UPDATE);
	*STORE_CENTER << *TargetPlayer;

//	system_log( "end page handler %s", get_name() );
	if (aPlayer->is_protected())
	{
	   aPlayer->clear_lost_planet();
	}

	return output("war/raid_result.html");
}

