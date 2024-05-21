#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePrivateerFleet::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	QUERY("TARGET_PLAYER_ID", TargetPlayerIDString);
	CHECK(TargetPlayerIDString == NULL,
			GETTEXT("You didn't enter a target player's ID."));

	int TargetPlayerID = as_atoi(TargetPlayerIDString);
	CPlayer *
		TargetPlayer = PLAYER_TABLE->get_by_game_id(TargetPlayerID);
	CHECK(!TargetPlayer,
			GETTEXT("That player doesn't exist.<BR>Please try again."));

	CHECK(TargetPlayer->get_game_id() == EMPIRE_GAME_ID,
			GETTEXT("You can't privateer in the Empire's domain."));

	CHECK(TargetPlayer->get_game_id() == aPlayer->get_game_id(),
			GETTEXT("You can't privateer in your own domain."));

	CHECK(TargetPlayer->is_dead(),
			(char *)format(GETTEXT("%1$s is dead."),
							TargetPlayer->get_nick()));

    CString Attack;
    Attack = aPlayer->check_attackableHi(TargetPlayer);
    CHECK(Attack.length(), Attack);

	CFleetList *FleetList = aPlayer->get_fleet_list();
	CHECK(FleetList->fleet_number_by_status(CFleet::FLEET_STAND_BY) == 0,
		GETTEXT("You don't have any stand-by fleets."));

	CCouncil *
		TargetPlayerCouncil = TargetPlayer->get_council();

    CRelation* Relation;
    CString RelationDescription;

    if (aPlayer->get_council() == TargetPlayer->get_council())
    {
        Relation = aPlayer->get_relation(TargetPlayer);

        if (Relation)
            RelationDescription = Relation->get_relation_description();
        else
            RelationDescription =
                CRelation::get_relation_description(CRelation::RELATION_NONE);
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
            RelationDescription =
                CRelation::get_relation_description(CRelation::RELATION_NONE);
		}
    }

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

	ITEM("STRING_RELATION", GETTEXT("Relation"));

	ITEM("RELATION", RelationDescription);

	ITEM("STRING_COUNCIL", GETTEXT("Council"));
	ITEM("COUNCIL_NICK", TargetPlayerCouncil->get_nick());

	ITEM("SELECT_FLEET_MESSAGE",
			GETTEXT("Select fleets to privateer."));

	ITEM("FLEETS_TO_SEND", FleetList->privateer_fleet_list_html(aPlayer));

	ITEM("TARGET_PLAYER_ID", TargetPlayerID);

	ITEM("PRIVATEER_INFORMATION_MESSAGE",
			GETTEXT("If you press the button below,<BR>"
					"your fleets will go into battle and will not return until the mission is complete.<BR>"
					"The order is irrevocable once you give the command.<BR>"
					"<BR>"
					"If you are really ready, send them to loot the enemy."));

//	system_log( "end page handler %s", get_name() );

	return output("war/privateer_fleet.html");
}

