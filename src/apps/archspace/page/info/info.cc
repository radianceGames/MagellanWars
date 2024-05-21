#include <libintl.h>
#include "../../pages.h"

bool
CPageInfo::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	ITEM("STRING_PLAYER_SEARCH", GETTEXT("Player Search"));

	ITEM("STRING_BY_ID_NUMBER", GETTEXT("By ID Number"));
	ITEM("STRING_BY_NAME", GETTEXT("Name"));

	ITEM("STRING_BY_COUNCIL_MEMBER", GETTEXT("Council Member"));

	CCouncil *
		Council = aPlayer->get_council();
	ITEM("MEMBER_SELECT", Council->select_member_html());

	ITEM("STRING_COUNCIL_SEARCH", GETTEXT("Council Search"));

	ITEM("STRING_BY_COUNCIL_ID_NUMBER", GETTEXT("By Council ID Number"));
	ITEM("STRING_BY_COUNCIL_NAME", GETTEXT("By Council Name"));

	ITEM("CLUSTERS_EXPLAIN",
			GETTEXT("Views all the clusters in Archspace."));

	ITEM("RANKING_EXPLAIN",
			GETTEXT("You can see the ranking in Archspace from various viewpoints."));

//	system_log("end page handler %s", get_name());

	return output("info/info.html");
}

