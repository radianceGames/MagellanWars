#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"

bool
CPageSiegePlanet::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

    //SLOG("Player %d attempted to load the siege_planet.cc", aPlayer->get_game_id());

	if (aPlayer->has_siege_blockade_restriction() == true)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Due to Imperial regulations, you may not siege for another %1$d seconds."), aPlayer->get_siege_blockade_restriction_time()));
		return output("war/war_error.html");
	}

	ITEM("STRING_TARGET", GETTEXT("Target"));
	ITEM("INFORMATION_MESSAGE",
			GETTEXT("You can only attack a player who is at war with you."));

	static CString TargetListHTML;
	TargetListHTML.clear();

	TargetListHTML = aPlayer->war_target_list_html();

	CHECK(!aPlayer->war_target_list_html(),
			GETTEXT("There is no target player to attack."));

	ITEM("TARGET_LIST", (char *)TargetListHTML);

//	system_log( "end page handler %s", get_name() );

	return output("war/siege_planet.html");
}

