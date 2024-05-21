#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"
#include "../../game.h"

bool
CPageInspectOtherPlayerResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("PLAYER_ID", PlayerIDString);
	CHECK(!PlayerIDString,
			GETTEXT("You didn't select any player."));

	int
		PlayerID = as_atoi(PlayerIDString);
	CPlayer *
		Player = (CPlayer *)PLAYER_TABLE->get_by_game_id(PlayerID);
	CHECK(!Player, GETTEXT("This player is not in your council"));
	if (Player->get_game_id() == EMPIRE_GAME_ID)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You can't inspect the Empire."));
		return output("diplomacy/inspect_other_player_error.html");
	}
	CHECK(Player->is_dead(), GETTEXT("This player is dead now."));

	ITEM("STRING_PLAYER_NAME", GETTEXT("Player Name"));
	ITEM("STRING_RELATION", GETTEXT("Relation"));
	ITEM("STRING_RANKING", GETTEXT("Ranking"));
	ITEM("STRING_HONOR", GETTEXT("Honor"));
	ITEM("STRING_POWER", GETTEXT("Power"));
	ITEM("STRING_NUMBER_OF_PLANET_S_", GETTEXT("Number of Planet(s)"));
	ITEM("STRING_COUNCIL", GETTEXT("Council"));

	ITEM("PLAYER_NICK", Player->get_nick());
	ITEM("PLAYER_ID", Player->get_game_id());

	CPlayerRelation *
		Relation = Player->get_relation(aPlayer);
	if(Relation)
	{
		ITEM("RELATION", CRelation::get_relation_description(Relation->get_relation()));
	} else
	{
		ITEM("RELATION", CRelation::get_relation_description(CRelation::RELATION_NONE));
	}

	CRankTable *
		RankTable = PLAYER_TABLE->get_overall_rank_table();
	int
		Rank = RankTable->get_rank_by_id(Player->get_game_id());
	if (Rank >= 1)
	{
		ITEM("RANKING", dec2unit(Rank));
	}
	else
	{
		ITEM("RANKING", GETTEXT("N/A"));
	}

	ITEM("HONOR", Player->get_honor_description());
	ITEM("POWER", dec2unit(Player->get_power()));
	ITEM("NUMBER_OF_PLANET_S_", Player->get_planet_list()->length());
	ITEM("COUNCIL_NICK", Player->get_council()->get_nick());
	ITEM("REPLY_MESSAGE",
			format("<A HREF=\"send_diplomatic_message_write.as?"
						"PLAYER_ID=%d\"><U>%s</U></A>",
				Player->get_game_id(),
				GETTEXT("Send a diplomatic message to this player")));

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/inspect_other_player_result.html" );
}

