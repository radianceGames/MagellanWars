#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageBlockadeFleet::handler(CPlayer *aPlayer)
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
	CHECK(!TargetPlayer,
			GETTEXT("That player doesn't exist.<BR>Please try again."));

	CHECK(TargetPlayer->get_game_id() == EMPIRE_GAME_ID,
			GETTEXT("You can't attack the Empire in this menu."));

	CHECK(TargetPlayer->get_game_id() == aPlayer->get_game_id(),
			GETTEXT("You can't blockade yourself."));

	CHECK(TargetPlayer->is_dead(),
				(char *)format(GETTEXT("%1$s is dead."),
								TargetPlayer->get_nick()));

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
		Relation;
    CString
		RelationDescription;


	Relation = aPlayer->get_relation(TargetPlayer);

	if (Relation)
		RelationDescription = Relation->get_relation_description();
	else
		RelationDescription = CRelation::get_relation_description(
							CRelation::RELATION_NONE);

	if(!Relation)
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

	CFleetList *FleetList = aPlayer->get_fleet_list();
	CHECK(FleetList->fleet_number_by_status(CFleet::FLEET_STAND_BY) == 0,
					GETTEXT("You don't have any stand-by fleets."));

	CPlanetList *
		TargetPlanetList = TargetPlayer->get_planet_list();
	CFleetList *
		TargetFleetList = TargetPlayer->get_fleet_list();


	ITEM("STRING_FLEET_DEPLOYMENT", GETTEXT("Fleet Deployment"));

	ITEM("TARGET_PLAYER",
			(char *)format(GETTEXT("Target Selected - %1$s"), TargetPlayer->get_nick()));

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

	ITEM("FLEETS_TO_SEND", FleetList->blockade_fleet_list_html(aPlayer));

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

	return output("war/blockade_fleet.html");
}

