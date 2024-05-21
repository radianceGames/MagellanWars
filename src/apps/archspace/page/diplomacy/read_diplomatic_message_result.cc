#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"

bool
CPagePlayerReadDiplomaticMessageResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("MESSAGE_ID", IDString);
	CHECK(!IDString,
			GETTEXT("There is no way to find a message"
					" you want to answer."
					" Please ask Archspace Development Team."));

	int MessageID = as_atoi(IDString);
	CDiplomaticMessage *Message =
			aPlayer->get_diplomatic_message(MessageID);
	CHECK(!Message, GETTEXT("Could not find such a message."));

	CHECK(Message->get_sender() == NULL, GETTEXT("The sender of the message does not exist."));
	CHECK(Message->get_sender()->is_dead(), GETTEXT("The sender of the message is dead."));

	QUERY("YOUR_COMMENT", YourCommentString);

	const char *YourComment;
	YourComment = (YourCommentString) ?
				YourCommentString : GETTEXT("(No comment)");

	if (Message->check_need_required())
	{
		QUERY("REPLY", ReplyString);
		CHECK(!ReplyString,
				GETTEXT("You have to answer this message."));

		if (!strcmp(ReplyString, "ACCEPT"))
		{
			if(Message->get_type() != CDiplomaticMessage::TYPE_SUGGEST_TRUCE)
			{
			CHECK((aPlayer->get_council_id() != ((Message->get_sender())->get_council_id())),
								GETTEXT("You are not in the same council as this player"));

			}
			CDiplomaticMessage::EType Type;

			if (Message->get_type() ==
						CDiplomaticMessage::TYPE_SUGGEST_TRUCE)
				Type = CDiplomaticMessage::TYPE_REPLY_SUGGEST_TRUCE;
			else if (Message->get_type() ==
						CDiplomaticMessage::TYPE_DEMAND_TRUCE)
				Type = CDiplomaticMessage::TYPE_REPLY_DEMAND_TRUCE;
			else if (Message->get_type() ==
						CDiplomaticMessage::TYPE_SUGGEST_PACT)
				Type = CDiplomaticMessage::TYPE_REPLY_SUGGEST_PACT;
			else if (Message->get_type() ==
						CDiplomaticMessage::TYPE_SUGGEST_ALLY)
				Type = CDiplomaticMessage::TYPE_REPLY_SUGGEST_ALLY;
			else {
				CHECK(true, GETTEXT("That message is an unknown type."));
			}

			const char *Ret =
					aPlayer->improve_relationship(Message->get_type(),
											Message->get_sender());
			CHECK(Ret != NULL, Ret);

			Message->set_status(CDiplomaticMessage::STATUS_ANSWERED);
			Message->type(QUERY_UPDATE);
			STORE_CENTER->store(*Message);

			CString Comment;
			Comment.format(GETTEXT("I accept your offer about %1$s."),
						Message->get_type_description());
			Comment += "\n";
			Comment.format("%s\n", YourComment);
			Comment += "---------------------------\n";
			Comment.format("%s %s\n",
							GETTEXT("Original message"),
							Message->get_type_description());
			Comment.format("%s %s, %s\n",
							GETTEXT("From"),
							Message->get_sender()->get_nick(),
							Message->get_time_string());
			Comment += Message->get_content();

			CDiplomaticMessage *
				SMessage = new CDiplomaticMessage();
			SMessage->initialize(Type, aPlayer, Message->get_sender(),
								(char *)Comment);
			Message->get_sender()->add_diplomatic_message(SMessage);
			SMessage->type(QUERY_INSERT);
			STORE_CENTER->store(*SMessage);

			CString Msg;
			Msg.format(GETTEXT("You successfully replied to the message(#%1$s) from %2$s."),
						dec2unit(Message->get_id()),
						Message->get_sender()->get_nick());

			Msg += "\n\n";
			Msg.format("%s\n", GETTEXT("Your Comment :"));
			Msg += (char *)Comment;

			Msg.htmlspecialchars();
			Msg.nl2br();

			ITEM("RESULT_MESSAGE", (char *)Msg);
		}
		else if (!strcmp(ReplyString, "REJECT"))
		{
			CDiplomaticMessage::EType Type;

			if (Message->get_type() ==
						CDiplomaticMessage::TYPE_SUGGEST_TRUCE)
				Type = CDiplomaticMessage::TYPE_REPLY_SUGGEST_TRUCE;
			else if (Message->get_type() ==
						CDiplomaticMessage::TYPE_DEMAND_TRUCE)
				Type = CDiplomaticMessage::TYPE_REPLY_DEMAND_TRUCE;
			else if (Message->get_type() ==
						CDiplomaticMessage::TYPE_SUGGEST_PACT)
				Type = CDiplomaticMessage::TYPE_REPLY_SUGGEST_PACT;
			else if (Message->get_type() ==
						CDiplomaticMessage::TYPE_SUGGEST_ALLY)
				Type = CDiplomaticMessage::TYPE_REPLY_SUGGEST_ALLY;
			else {
				CHECK(true, GETTEXT("That message is an unknown type."));
			}

			Message->set_status(CDiplomaticMessage::STATUS_ANSWERED);
			Message->type(QUERY_UPDATE);
			STORE_CENTER->store(*Message);

			CString Comment;
			Comment.format(GETTEXT("I reject your offer about %1$s."),
						Message->get_type_description());
			Comment += "\n";
			Comment.format("%s\n", YourComment);

			Comment += "---------------------------\n";
			Comment.format("%s %s\n",
							GETTEXT("Original message"),
							Message->get_type_description());
			Comment.format("%s %s, %s\n",
							GETTEXT("From"),
							Message->get_sender()->get_nick(),
							Message->get_time_string());
			Comment += Message->get_content();

			CDiplomaticMessage *
				SMessage = new CDiplomaticMessage();
			SMessage->initialize(Type, aPlayer, Message->get_sender(),
								(char*)Comment);
			Message->get_sender()->add_diplomatic_message(SMessage);
			SMessage->type(QUERY_INSERT);
			STORE_CENTER->store(*SMessage);

			CString Msg;
			Msg.format(GETTEXT("You successfully replied to the message(#%1$s) from %2$s."),
						dec2unit(Message->get_id()),
						Message->get_sender()->get_nick());

			Msg += "\n\n";
			Msg.format("%s\n", GETTEXT("Your Comment :"));
			Msg += (char *)Comment;

			Msg.htmlspecialchars();
			Msg.nl2br();

			ITEM("RESULT_MESSAGE", (char *)Msg);
		}
		else if (!strcmp(ReplyString, "WELL"))
		{
			Message->set_status(CDiplomaticMessage::STATUS_WELL);
			Message->type(QUERY_UPDATE);
			STORE_CENTER->store(*Message);

			ITEM("RESULT_MESSAGE",
				GETTEXT("You hold off your decision to reply this message."));
		}
		else
		{
			CHECK(true, GETTEXT("Could not understand your answer."));
		}
	}
	else
	{
		CString
			Comment;
		Comment.clear();
		Comment.format("%s\n", YourComment);

		Comment += "---------------------------\n";
		Comment.format("%s %s\n",
						GETTEXT("Original message"),
						Message->get_type_description());
		Comment.format("%s %s, %s\n",
						GETTEXT("From"),
						Message->get_sender()->get_nick(),
						Message->get_time_string());
		Comment += Message->get_content();

		CDiplomaticMessage *
			SMessage = new CDiplomaticMessage();
		SMessage->initialize(CDiplomaticMessage::TYPE_REPLY,
					aPlayer, Message->get_sender(), (char*)Comment);
		Message->get_sender()->add_diplomatic_message(SMessage);
		SMessage->type(QUERY_INSERT);
		STORE_CENTER->store(*SMessage);

		CString Msg;
		Msg.format(GETTEXT("You successfully replied to the message(#%1$s) from %2$s."),
					dec2unit(Message->get_id()),
					Message->get_sender()->get_nick());

		Msg += "\n\n";
		Msg.format("%s\n", GETTEXT("Your Comment :"));
		Msg += (char *)Comment;

		Msg.htmlspecialchars();
		Msg.nl2br();

		ITEM("RESULT_MESSAGE", (char *)Msg);
	}

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/read_diplomatic_message_result.html" );
}

