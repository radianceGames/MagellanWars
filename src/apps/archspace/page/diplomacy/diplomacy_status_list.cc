#include <libintl.h>
#include "../../pages.h"

bool
CPagePlayerDiplomacyStatusList::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CCouncil *
		Council = aPlayer->get_council();
	const char *RelationTable = 
			Council->player_relation_table_html(aPlayer);

	if (!RelationTable)
	{
		ITEM("STATUS_TABLE",
				GETTEXT("There are no players who you know."));
	}
	else
	{
		ITEM("STATUS_TABLE", RelationTable);
	}

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/diplomacy_status_list.html" );
}

