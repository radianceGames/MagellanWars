#include <libintl.h>
#include "../../pages.h"

bool
CPageSpyChangeSecurityLevel::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	ITEM("INFORMATION_MESSAGE",
			GETTEXT("Higher security level requires higher upkeep."));

	ITEM("STRING_CHANGE_SECURITY_LEVEL",
			GETTEXT("Change Security Level"));

	ITEM("SECURITY_LEVEL_SELECT", CSpy::security_level_select_html());

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/spy_change_security_level.html" );
}
