#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerSearch::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("BY_WHAT", ByWhatString);
	if (!ByWhatString)
	{
		message_page("You didn't specify the way how to search a player.");
		return true;
	}

	CPlayer *
		Player = NULL;

	if (!strcmp(ByWhatString, "PLAYER_NAME"))
	{
		QUERY("PLAYER_NAME", PlayerNameString);

		if (!PlayerNameString)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You didn't enter a player name."));
			return output("info/player_search_error.html");
		}

		static CString
			NameList;
		NameList.clear();

		bool
			AnyPlayer = false;
		for (int i=0 ; i<PLAYER_TABLE->length() ; i++)
		{
			CPlayer *
				Player = (CPlayer *)PLAYER_TABLE->get(i);
			if (strstr(Player->get_name(), PlayerNameString))
			{
				NameList += "<BR>\n";
				NameList.format("<A HREF=\"player_search_result.as?PLAYER_ID=%d\">%s</A>\n",
								Player->get_game_id(), Player->get_nick());

				AnyPlayer = true;
			}
		}

		if (AnyPlayer)
		{
			ITEM("SELECT_PLAYER_MESSAGE",
					GETTEXT("Select the player you want from the following list."));

			ITEM("PLAYER_LIST", (char *)NameList);

			return output("info/player_search_name_list.html");
		} else
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("That player doesn't exist."));
			return output("info/player_search_error.html");
		}
	}

	if (!strcmp(ByWhatString, "PLAYER_ID"))
	{
		QUERY("PLAYER_ID", PlayerIDString);
		int
			PlayerID = as_atoi(PlayerIDString);
		Player = PLAYER_TABLE->get_by_game_id(PlayerID);
	} else if (!strcmp(ByWhatString, "MEMBER_ID"))
	{
		QUERY("MEMBER", MemberString);
		int
			PlayerID = as_atoi(MemberString);
		Player = PLAYER_TABLE->get_by_game_id(PlayerID);
	}

	if (!Player)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("That player doesn't exist."));
		return output("info/player_search_error.html");
	}
	if (Player->get_game_id() == EMPIRE_GAME_ID)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You can't inspect the Empire."));
		return output("info/player_search_error.html");
	}
	if (Player->is_dead())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("That player is dead now."));
		return output("info/player_search_error.html");
	}

	ITEM("STRING_NAME", GETTEXT("Name"));
	ITEM("NAME", Player->get_nick());

	ITEM("STRING_RACE", GETTEXT("Race"));
	ITEM("RACE", Player->get_race_name());

	CRelation *
		Relation = aPlayer->get_relation(Player);
	if (Relation)
	{
		ITEM("STRING_RELATION", GETTEXT("Relation"));
		ITEM("RELATION", Relation->get_relation_description());
	} else
	{
		ITEM("STRING_RELATION", GETTEXT("Relation"));
		ITEM("RELATION",
				CRelation::get_relation_description(CRelation::RELATION_NONE));
	}

	ITEM("STRING_HONOR", GETTEXT("Honor"));
	ITEM("HONOR", Player->get_honor());

	ITEM("STRING_POWER", GETTEXT("Power"));
	ITEM("POWER", dec2unit(Player->get_power()));

	CPlanetList *
		PlanetList = Player->get_planet_list();

	ITEM("STRING_PLANETS", GETTEXT("Planets"));
	ITEM("PLANETS", PlanetList->length());

	ITEM("STRING_FLEET_S_", GETTEXT("Fleet(s)"));
	ITEM("FLEETS", GETTEXT("N/A"));

	CCouncil *
		Council = Player->get_council();

	ITEM("STRING_COUNCIL", GETTEXT("Council"));
	ITEM("COUNCIL", Council->get_nick());
	
	ITEM("STRING_RANKING", GETTEXT("Ranking"));

	ITEM("STRING_CLUSTERS", Player->get_cluster_list_string());

	CRankTable *
		RankTable = PLAYER_TABLE->get_overall_rank_table();
	int
		Rank = RankTable->get_rank_by_id(Player->get_game_id());
	if (Rank >= 1)
	{
		ITEM("RANK_VALUE", dec2unit(Rank));
	}
	else
	{
		ITEM("RANK_VALUE", GETTEXT("N/A"));
	}

    if (Player->get_purchased_project_list()->get_by_id(9010 + Player->get_race())) {
        ITEM("ENDING_VERSION", GETTEXT("a"));
    } else if (Player->get_purchased_project_list()->get_by_id(9010 + Player->get_race())) {
        ITEM("ENDING_VERSION", GETTEXT("b"));
    } else {
        ITEM("ENDING_VERSION", GETTEXT("n"));
    }

//	system_log("end page handler %s", get_name());

	return output("info/player_search_result.html");
}

