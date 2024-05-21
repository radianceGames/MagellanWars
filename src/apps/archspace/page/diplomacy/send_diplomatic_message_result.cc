#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerSendDiplomaticMessageResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("PLAYER_ID", PlayerString);
	int
		PlayerID = as_atoi(PlayerString);

	CHECK(PlayerID == EMPIRE_GAME_ID, GETTEXT("Sending a message to the empire is prohibited."));

	CPlayer *
		Player = PLAYER_TABLE->get_by_game_id(PlayerID);

	CHECK(!Player, GETTEXT("This player doesn't exist."));

	CHECK(Player->get_game_id() == aPlayer->get_game_id(),
			GETTEXT("You can't send a message to yourself."));

    CHECK(Player->is_dead(), GETTEXT("This player is dead now."));

	QUERY("MESSAGE_TYPE", MessageTypeString);

	CHECK(!MessageTypeString,
			GETTEXT("Message type is not specified."));

	QUERY("COMMENT", Comment);

	CDiplomaticMessage::EType
		MessageType = (CDiplomaticMessage::EType)as_atoi(MessageTypeString);

	CHECK(!aPlayer->check_diplomatic_message(MessageType, Player),
		format(GETTEXT("The message type(%1$s) is not available."),
			CDiplomaticMessage::get_type_description(MessageType)));

	CHECK(Player->check_duplication_diplomatic_message(MessageType, aPlayer),
		GETTEXT("You already sent this kind of diplomatic message."));

	CDiplomaticMessage *
		Message = new CDiplomaticMessage();
	Message->initialize(MessageType, aPlayer, Player, Comment);
	Player->add_diplomatic_message(Message);

	Message->CStore::type(QUERY_INSERT);
	STORE_CENTER->store(*Message);

	ITEM("RESULT_MESSAGE",
			(char *)format(GETTEXT("You successfully sent a message to %1$s."),
							Player->get_nick()));

	if (MessageType == CDiplomaticMessage::TYPE_DECLARE_WAR)
	{
		aPlayer->declare_war(Player);

		ADMIN_TOOL->add_player_relation_log(
				(char *)format("The player %s has declared war against the player %s.",
								aPlayer->get_nick(), Player->get_nick()));
	}

	if (MessageType == CDiplomaticMessage::TYPE_BREAK_PACT)
	{
		aPlayer->break_pact(Player);

		ADMIN_TOOL->add_player_relation_log(
				(char *)format("The player %s has broken pact against the player %s.",
								aPlayer->get_nick(), Player->get_nick()));
	}

	if (MessageType == CDiplomaticMessage::TYPE_BREAK_ALLY)
	{
		aPlayer->break_ally(Player);

		ADMIN_TOOL->add_player_relation_log(
				(char *)format("The player %s has broken ally against the player %s.",
								aPlayer->get_nick(), Player->get_nick()));
	}
	if (MessageType == CDiplomaticMessage::TYPE_DECLARE_HOSTILITY)
		{
			aPlayer->declare_hostile(Player);

			ADMIN_TOOL->add_player_relation_log(
					(char *)format("The player %s has Declared Hostile Intent toward the player %s.",
									aPlayer->get_nick(), Player->get_nick()));
	}

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/send_diplomatic_message_result.html" );
}

