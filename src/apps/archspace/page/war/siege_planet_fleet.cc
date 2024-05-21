#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageSiegePlanetFleet::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

  	// if player doesn't have a preference object yet, make it
    	if (aPlayer->get_preference() == NULL)
        	aPlayer->set_preference(new CPreference(aPlayer->get_game_id()));

	CPreference *
		aPreference = aPlayer->get_preference();

	if (aPlayer->has_siege_blockade_restriction() == true)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Due to Imperial regulations, you may not siege for another %1$d seconds."), aPlayer->get_siege_blockade_restriction_time()));

		return output("war/war_error.html");
	}

	QUERY("TARGET_PLAYER_ID", TargetPlayerIDString);
	CHECK(TargetPlayerIDString == NULL,
			GETTEXT("You didn't enter a target player's ID."));

	int
		TargetPlayerID = as_atoi(TargetPlayerIDString);

	CPlayer *
		TargetPlayer = PLAYER_TABLE->get_by_game_id(TargetPlayerID);

	CHECK(!TargetPlayer,GETTEXT("There's no such a player."));

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

	CCouncil *
		TargetPlayerCouncil = TargetPlayer->get_council();

	CRelation *
		Relation = aPlayer->get_relation(TargetPlayer);
	CString
		RelationDescription;
	if (Relation != NULL && Relation->get_relation() == CRelation::RELATION_BOUNTY)
	{
/*		if (TargetPlayer->get_planet_list()->length() == 1)
		{
			ITEM("ERROR_MESSAGE",
                        (char *)format("The player cannot be killed through a bounty"));

                        return output("war/war_error.html");
		}*/
		RelationDescription = GETTEXT("Bounty");
	}
	else if (aPlayer->get_council() == TargetPlayerCouncil)
	{
		CHECK(!Relation,
			(char*)format(GETTEXT("You have no relation with %1$s."),
								TargetPlayer->get_nick()));

			CHECK((Relation->get_relation() != CRelation::RELATION_WAR && Relation->get_relation() != CRelation::RELATION_TOTAL_WAR),
			(char*)format(GETTEXT("You are not at war with %1$s."),
						TargetPlayer->get_nick()));
		RelationDescription = GETTEXT("Personal War");
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
				(char *)format(GETTEXT("You have not been at war with the player %1$s long enough to attack. Please wait another %2$s seconds"),
			TargetPlayer->get_nick(), dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
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
			else if((CGame::get_game_time() - Relation->get_time()) < 600)
						{
							ITEM("ERROR_MESSAGE",
												(char *)format(GETTEXT("You have not been at war with the player %1$s long enough to attack. Please wait another %2$s seconds"),
							TargetPlayer->get_nick(), dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
			return output("war/war_error.html");
			}
			RelationDescription = GETTEXT("Council War");
		}
		else
		{
			if((CGame::get_game_time() - Relation->get_time()) < 600)
						{
							ITEM("ERROR_MESSAGE",
												(char *)format(GETTEXT("You have not been at war with the player %1$s long enough to attack. Please wait another %2$s seconds"),
							TargetPlayer->get_nick(), dec2unit((Relation->get_time() + 600) - CGame::get_game_time()) ));
			return output("war/war_error.html");
			}
			RelationDescription = GETTEXT("Council Total War");
		}
	}

	CFleetList *
		FleetList = aPlayer->get_fleet_list();
	CHECK(FleetList->fleet_number_by_status(CFleet::FLEET_STAND_BY) == 0,
			GETTEXT("You don't have any stand-by fleets."));

	CPlanetList *
		TargetPlanetList = TargetPlayer->get_planet_list();
	CFleetList *
		TargetFleetList = TargetPlayer->get_fleet_list();


	ITEM("STRING_FLEET_DEPLOYMENT", GETTEXT("Fleet Deployment"));

	ITEM("TARGET_PLAYER",
			(char *)format(GETTEXT("Target Selected - %1$s"),
					TargetPlayer->get_nick()));

	ITEM("STRING_RANKING", GETTEXT("Ranking"));

	CRankTable *
		RankTable = PLAYER_TABLE->get_overall_rank_table();
	int
		Rank = RankTable->get_rank_by_id(TargetPlayer->get_game_id());
	ITEM("RANKING", dec2unit(Rank));

	ITEM("STRING_POWER", GETTEXT("Power"));
	ITEM("POWER", dec2unit(TargetPlayer->get_power()));

	ITEM("STRING_NUMBER_OF_PLANET_S_", GETTEXT("Number of Planet(s)"));
	ITEM("NUMBER_OF_PLANETS", dec2unit(TargetPlanetList->length()));

	ITEM("STRING_NUMBER_OF_FLEET_S_", GETTEXT("Number of Fleet(s)"));
	ITEM("NUMBER_OF_FLEET_S_", dec2unit(TargetFleetList->length()));

	ITEM("STRING_RELATION", GETTEXT("Relation"));
	ITEM("RELATION", RelationDescription);

	ITEM("STRING_COUNCIL", GETTEXT("Council"));
	ITEM("COUNCIL_NICK", TargetPlayerCouncil->get_nick());

	ITEM("SELECT_FLEET_MESSAGE",
			GETTEXT("Select fleets to deploy(up to 20 fleets)."));

	ITEM("FLEETS_TO_SEND", FleetList->siege_planet_fleet_list_html(aPlayer));

	ITEM("TARGET_PLAYER_ID", TargetPlayerID);

	ITEM("PREFERENCE_PREFIX", GETTEXT("Deploy fleets using:"));

	CString HTMLSelectOptions;
	HTMLSelectOptions.clear();
	for (int i=0; i <= CPreference::PR_JAVASCRIPT; i++)
    {
      if (aPreference->has(i))
          HTMLSelectOptions.format("<OPTION VALUE=%d SELECTED>%s</OPTION>\n",i,aPreference->EPreferencetoString((CPreference::EPreference)i));
      else
          HTMLSelectOptions.format("<OPTION VALUE=%d>%s</OPTION>\n",i,aPreference->EPreferencetoString((CPreference::EPreference)i));
    }

	ITEM("SELECT_OPTIONS", (char*)HTMLSelectOptions);

//	system_log( "end page handler %s", get_name() );

	return output("war/siege_planet_fleet.html");
}

