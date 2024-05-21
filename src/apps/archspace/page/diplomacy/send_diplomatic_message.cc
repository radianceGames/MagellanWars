#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"

void
CPagePlayerSendDiplomaticMessage::message_page(const char *aMessage)
{
	ITEM("RESULT_MESSAGE", aMessage);
	output("diplomacy/send_diplomatic_message_result.html");
}


bool
CPagePlayerSendDiplomaticMessage::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CCouncil *
		Council = aPlayer->get_council();

	ITEM("SELECT_PLAYER_NAME_MESSAGE",
			GETTEXT("Select the name of the player you want to send a message to."));

	const char *
		SelectPlayer = Council->select_member_except_player_html(aPlayer);

	if (SelectPlayer != NULL)
	{
		ITEM("SELECT_PLAYER", SelectPlayer);
		ITEM("OK_BUTTON",
				(char *)format("<INPUT TYPE=image SRC=\"%s/image/as_game/bu_ok.gif\" BORDER=0>",
								(char *)CGame::mImageServerURL));
	}
	else
	{
		ITEM("SELECT_PLAYER", GETTEXT("You are the only member in your council."));
		ITEM("OK_BUTTON", " ");
	}

	ITEM("INPUT_PLAYER_NUMBER_MESSAGE",
			GETTEXT("Or enter the ID number of the player you want to send a message to."));

	ITEM("STRING_PLAYER__", GETTEXT("Player #"));

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/send_diplomatic_message.html" );
}

