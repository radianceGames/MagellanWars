#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageRankCouncilFleet::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CRankTable *
		RankTable = COUNCIL_TABLE->get_fleet_rank_table();
	CCouncil *
		Council = aPlayer->get_council();

	ITEM("TOP_TEN_RANK_NAME", GETTEXT("Leading Councils"));
	ITEM("TOP_TEN_RANK",
			RankTable->top_ten_info_html(Council->get_id()));

	ITEM("AROUND_ME_RANK_NAME",
			(char *)format(GETTEXT("Councils With Similar Fleet Power Near %1$s"),
							Council->get_nick()));
	ITEM("AROUND_ME_RANK",
			RankTable->around_me_info_html(Council->get_id()));

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

