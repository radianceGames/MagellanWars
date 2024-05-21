#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageRankPlayerTech::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CRankTable *
		RankTable = PLAYER_TABLE->get_tech_rank_table();

	ITEM("TOP_TEN_RANK_NAME", GETTEXT("Leading Players"));
	ITEM("TOP_TEN_RANK",
			RankTable->top_ten_info_html(aPlayer->get_game_id()));

	ITEM("AROUND_ME_RANK_NAME",
			(char *)format(GETTEXT("Players With Similar Technical Advancement Near %1$s"),
							aPlayer->get_nick()));
	ITEM("AROUND_ME_RANK",
			RankTable->around_me_info_html(aPlayer->get_game_id()));

	char TimeString[60];
	time_t
		UpdatedTime = RankTable->get_time();
	struct tm *
		Time = localtime(&UpdatedTime);
	strftime(TimeString, 60, " %Y/%m/%d&nbsp;%r", Time);

	ITEM("UPDATED_TIME",
			(char *)format(GETTEXT("Last updated : %1$s"), TimeString));

//	system_log("end page handler %s", get_name());

	return output("info/rank_table.html");
}

