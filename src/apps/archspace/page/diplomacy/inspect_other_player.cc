#include <libintl.h>
#include "../../pages.h"

void
CPageInspectOtherPlayer::message_page(const char *aMessage)
{
	ITEM("ERROR_MESSAGE", aMessage);
	output("diplomacy/inspect_other_player_error.html");
}

bool
CPageInspectOtherPlayer::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CCouncil *
		Council = aPlayer->get_council();
	if (Council->get_number_of_members() == 1)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There are no another players in your council."));
		return output( "diplomacy/inspect_other_player_error.html" );
	}

	ITEM("SELECT_PLAYER_NAME_MESSAGE",
			GETTEXT("Select the name of the player you want to inspect."));
	ITEM("SELECT_PLAYER_NAME",
			aPlayer->get_council()->
				select_member_except_player_html(aPlayer));

	ITEM("INPUT_PLAYER_NUMBER_MESSAGE",
			GETTEXT("Or enter the ID number of the player you want to inspect."));

	ITEM("STRING_PLAYER__", GETTEXT("Player #"));

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/inspect_other_player.html" );
}

