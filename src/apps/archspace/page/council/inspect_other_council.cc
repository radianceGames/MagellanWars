#include <libintl.h>
#include "../../pages.h"

bool
CPageInspectOtherCouncil::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	ITEM("MENU_TITLE", GETTEXT("Council Diplomacy - Inspect Other Council"));

	ITEM("SELECT_COUNCIL_MESSAGE",
			GETTEXT("Select the name of the council you want to inspect."));

	ITEM("COUNCIL_LIST", aPlayer->get_council()->select_council_html());

	ITEM("ENTER_NUMBER_MESSAGE",
			GETTEXT("Or enter the Cluster number and the number of the council "
					"you want to inspect."));

	ITEM("STRING_COUNCIL__", GETTEXT("Council #"));

//	system_log("end page handler %s", get_name());

	return output("council/inspect_other_council.html");
}
