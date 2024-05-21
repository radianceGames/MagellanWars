#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"

bool
CPageReadDiplomaticMessageUnanswered::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("MESSAGE_ID", MessageIDString);
	int
		MessageID = as_atoi(MessageIDString);

	CDiplomaticMessage *Message =
				aPlayer->get_diplomatic_message(MessageID);
	CHECK(!Message,
			(char *)format(GETTEXT("The message with ID %1$s doesn't exist."),
							dec2unit(MessageID)));

	ITEM("STRING_NO_", GETTEXT("No."));
	ITEM("STRING_SENDER", GETTEXT("Sender"));
	ITEM("STRING_DATE", GETTEXT("Date"));
	ITEM("STRING_MESSAGE_TYPE", GETTEXT("Message Type"));
	ITEM("STRING_REPLY_STATUS", GETTEXT("Reply Status"));
	ITEM("STRING_COMMENT", GETTEXT("Comment"));

	ITEM("NO_", Message->get_id());
	if(Message->get_sender() == NULL)
	{
		ITEM("FROM", GETTEXT("N/A"));
	}
	else if (Message->get_sender()->get_game_id() == EMPIRE_GAME_ID)
	{
		ITEM("FROM", GETTEXT("GM"));
	}
	else
	{
		ITEM("FROM", format(
				"<A HREF=\"inspect_other_player_result.as?PLAYER_ID=%d\">"
				"%s</A>\n", 
				Message->get_sender()->get_game_id(), 
				Message->get_sender()->get_nick()));
	}

	ITEM("DATE", Message->get_time_string());
	ITEM("MESSAGE_TYPE", Message->get_type_description());
	ITEM("REPLY_STATUS", Message->get_status_description());
	CString
		Comment;
	Comment = Message->get_content();
	ITEM("COMMENT", (char *)nl2br((char *)htmlspecialchars((char *)Comment)));

	if(Message->get_sender() == NULL)
	{
		ITEM("REPLY_TABLE", " ");

		ITEM("LINK",
				(char *)format("<TR>\n<TD CLASS=\"maintext\" ALIGN=\"center\"><A HREF=read_diplomatic_message.as><IMG SRC=\"%s/image/as_game/bu_list_1.gif\" BORDER=0 WIDTH=\"37\" HEIGHT=\"11\"></A>\n</TR>", (char *)CGame::mImageServerURL));
	}
	else if (Message->get_sender()->get_game_id() == EMPIRE_GAME_ID)
	{
		ITEM("REPLY_TABLE", " ");
		ITEM("LINK",
				(char *)format("<TR>\n<TD CLASS=\"maintext\" ALIGN=\"center\"><A HREF=read_diplomatic_message.as><IMG SRC=\"%s/image/as_game/bu_list_1.gif\" BORDER=0 WIDTH=\"37\" HEIGHT=\"11\"></A>\n</TR>", (char *)CGame::mImageServerURL));
	}
	else
	{
		ITEM("REPLY_TABLE", Message->get_reply_html());	
		ITEM("LINK",
				(char *)format("<TR>\n<TD CLASS=\"maintext\" ALIGN=\"center\"><INPUT TYPE=image SRC=\"%s/image/as_game/bu_send_1.gif\" BORDER=\"0\">&nbsp;<A HREF=read_diplomatic_message.as><IMG SRC=\"%s/image/as_game/bu_list_1.gif\" BORDER=0 WIDTH=\"37\" HEIGHT=\"11\"></A>\n</TR>", (char *)CGame::mImageServerURL, (char *)CGame::mImageServerURL));
	}

	CString
		Accuse;
	Accuse.clear();

	Accuse.format("<A HREF=\"diplomatic_message_accuse.as?"
												"MESSAGE_ID=%d\">",
					Message->get_id());
	Accuse += GETTEXT("Accuse the sender of improper language.");
	Accuse += "</A>";

	ITEM("ACCUSE_MESSAGE", (char *)Accuse);

	if (Message->get_status() == CDiplomaticMessage::STATUS_UNREAD)
	{
		Message->set_status(CDiplomaticMessage::STATUS_READ);
		Message->type(QUERY_UPDATE);
		STORE_CENTER->store(*Message);
	}
//	system_log("end page handler %s", get_name());

	return output( "diplomacy/read_diplomatic_message_unanswered.html" );
}

