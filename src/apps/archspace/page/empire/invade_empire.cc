#include <libintl.h>
#include "../../pages.h"

bool
CPageEmpireInvadeEmpire::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	ITEM("STRING_MAGISTRATE", GETTEXT("Magistrate"));
	ITEM("STRING_EMPIRE_PLANET", GETTEXT("Empire Planet"));
	ITEM("STRING_FORTRESS", GETTEXT("Fortress"));
	ITEM("STRING_EMPIRE_CAPITAL_PLANET", GETTEXT("Empire Capital Planet"));

//	system_log( "end page handler %s", get_name() );

	return output("empire/invade_empire.html");
}

