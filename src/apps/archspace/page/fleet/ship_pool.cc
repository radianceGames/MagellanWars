#include <libintl.h>
#include "../../pages.h"

bool
CPageShipPool::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	ITEM("RESULT_MESSAGE", " ");

	ITEM("STRING_SHIPS_IN_THE_POOL", GETTEXT("Ships in the Pool"));

	ITEM("POOL_INFO", aPlayer->get_dock()->dock_info_html());

	ITEM("STRING_REPAIRING_BAY", GETTEXT("Repairing Bay"));

	ITEM("REPAIRING_BAY_INFORMATION",
			aPlayer->get_repair_bay()->repairing_bay_info_html());

	ITEM("STRING_CLASS_SPECIFICATION", GETTEXT("Class Specification"));

	ITEM("CLASS_INFORMATION", aPlayer->get_ship_design_list()->print_html(aPlayer));

//	system_log( "end page handler %s", get_name() );

	return output("fleet/ship_pool.html");
}

