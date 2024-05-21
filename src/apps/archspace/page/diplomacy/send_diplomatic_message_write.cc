#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerSendDiplomaticMessageWrite::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CCouncil *
		Council = aPlayer->get_council();

	QUERY("PLAYER_ID", PlayerString);
	int
		PlayerID = as_atoi(PlayerString);

	CHECK(PlayerID == EMPIRE_GAME_ID, 
		GETTEXT("Sending a message to the empire is prohibited."));

	CPlayer * Player;
	Player = PLAYER_TABLE->get_by_game_id(PlayerID);

	CHECK(!Player, 
		GETTEXT("No such player exists."));

	CHECK(Player->get_game_id() == aPlayer->get_game_id(),
			GETTEXT("You can't send a message to yourself."));

    CHECK(Player->is_dead(), GETTEXT("This player is dead now."));

	ITEM("STRING_SENDER", GETTEXT("Sender"));
	ITEM("STRING_TO", GETTEXT("To"));
	ITEM("STRING_MESSAGE_TYPE", GETTEXT("Message Type"));
	ITEM("STRING_COMMENT", GETTEXT("Comment"));

	ITEM("PLAYER_NICK", aPlayer->get_nick());

	ITEM("TO", format(
		"<A HREF=\"inspect_other_player_result.as?PLAYER_ID=%d\">%s</A>"
		"<INPUT TYPE=hidden NAME=PLAYER_ID VALUE=%d>",
			Player->get_game_id(), Player->get_nick(), 
			Player->get_game_id()));

	ITEM("SELECT_MESSAGE_TYPE", 
				aPlayer->diplomatic_message_select_html(Player));

	ITEM("SELECT_PLAYER",
			Council->select_member_except_player_html(aPlayer));

	ITEM("INPUT_PLAYER_NUMBER_MESSAGE",
			GETTEXT("Or enter the ID number of the player you want to send a message to."));

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/send_diplomatic_message_write.html" );
}

