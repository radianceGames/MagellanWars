#include <libintl.h>
#include "../../pages.h"

bool
CPageCouncilDiplomacyStatusList::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	CCouncil *Council = aPlayer->get_council();
	ITEM("MENU_TITLE", format(GETTEXT("Council Diplomacy - %1$s Status List"),
								Council->get_nick()));

	ITEM("STRING_RELATION", GETTEXT("Relation"));
	ITEM("STRING_COUNCIL", GETTEXT("Council"));

	ITEM("RELATION_TABLE", Council->council_relation_table_html());

//	system_log("end page handler %s", get_name());

	return output("council/diplomacy_status_list.html");
}
