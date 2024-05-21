#include <libintl.h>
#include "../../pages.h"

bool
CPageShipDesignNotregister::handler( CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	ITEM("NOT_REGISTER_MESSAGE",
			GETTEXT("You dropped this ship design, and it's not registered."));

//	system_log("end page handler %s", get_name());

	return output("fleet/ship_design_notregister.html");
}

