#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageShipDesign::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	ITEM("RESULT_MESSAGE", " ");

	ITEM("STRING_NEW_CLASS_NAME", GETTEXT("New Class Name"));
	ITEM("STRING_SIZE", GETTEXT("Size"));

	ITEM("AVAILABLE_SIZE_LIST", SHIP_SIZE_TABLE->available_size_list_html(aPlayer));

	ITEM("STRING_SIZE_LIST", GETTEXT("Size List"));

	ITEM("SIZE_INFORMATION", SHIP_SIZE_TABLE->size_information_html(aPlayer));

	ITEM("STRING_PREVIOUS_DESIGN", GETTEXT("Previous Design"));

	ITEM("CLASS_INFORMATION", aPlayer->get_ship_design_list()->print_html(aPlayer));

//	system_log( "end page handler %s", get_name() );

	return output("fleet/ship_design.html");
}

