#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"

bool
CPageDiplomaticMessageRead::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	QUERY("MESSAGE_ID", MessageIDString);
	int
		MessageID = as_atoi(MessageIDString);
	CCouncilMessage *Message = 
			aPlayer->get_council()->get_council_message(MessageID);
	CHECK(!Message,
			(char *)format(GETTEXT("The message with ID %1$s doesn't exist."),
							dec2unit(MessageID)));

	ITEM("MENU_TITLE", GETTEXT("Council Diplomacy"));

	ITEM("STRING_NO_", GETTEXT("No."));
	ITEM("STRING_SENDER", GETTEXT("Sender"));
	ITEM("STRING_DATE", GETTEXT("Date"));
	ITEM("STRING_MESSAGE_TYPE", GETTEXT("Message Type"));
	ITEM("STRING_REPLY_STATUS", GETTEXT("Status"));
	ITEM("STRING_COMMENT", GETTEXT("Comment"));

	ITEM("NO_", Message->get_id());
	if (Message->get_sender() == NULL)
	{
		ITEM("COUNCIL_NICK", GETTEXT("N/A"));
	}
	else
	{
		ITEM("COUNCIL_NICK", (char *)format("<A HREF = \"inspect_other_council_result.as?COUNCIL_ID=%d\">%s</A>", 
		Message->get_sender()->get_id(), Message->get_sender()->get_nick()));
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
	}
	else
	{
		ITEM("REPLY_TABLE", Message->get_reply_html());
	}

	if(Message->get_sender() == NULL)
	{
		ITEM("SEND_MESSAGE_LINK", " ");
	}
	else
	{
		ITEM("SEND_MESSAGE_LINK", 
				(char *)format("<INPUT TYPE=IMAGE SRC=\"%s/image/as_game/bu_send_1.gif\" WIDTH=\"45\" HEIGHT=\"11\" BORDER=\"0\">", (char *)CGame::mImageServerURL));
	}
		
	CString
		Accuse;
	Accuse.clear();

	Accuse.format("<A HREF=\"diplomatic_message_accuse.as?MESSAGE_ID=%d\">",
					Message->get_id());
	Accuse += GETTEXT("Accuse the sender of improper language.");
	Accuse += "</A>";

	ITEM("ACCUSE_MESSAGE", (char *)Accuse);

	if (Message->get_status() == CCouncilMessage::STATUS_UNREAD)
	{
		Message->set_status(CCouncilMessage::STATUS_READ);
		Message->type(QUERY_UPDATE);
		STORE_CENTER->store(*Message);
	}

//	system_log("end page handler %s", get_name());

	return output("council/diplomatic_message_read.html");
}
