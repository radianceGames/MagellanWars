#include <libintl.h>
#include "../../pages.h"

bool
CPageDiplomaticMessage::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	ITEM("MENU_TITLE", GETTEXT("Council Diplomacy"));

	static CString
		MessageList;
	MessageList.clear();
	MessageList = aPlayer->get_council()->get_message_list_html(0, false);

	if (MessageList.length() > 0)
	{
		ITEM("COUNCIL_MESSAGE_LIST", (char *)MessageList);
	}
	else
	{
		CString
			Message;
		Message.clear();

		Message = "<TD CLASS=\"maintext\" ALIGN=CENTER COLSPAN=3>\n";
		Message += GETTEXT("You have received no diplomatic messages.");
		Message += "</TD>\n";

		ITEM("COUNCIL_MESSAGE_LIST", (char *)Message);
	}

//	system_log("end page handler %s", get_name());

	return output("council/diplomatic_message.html");
}

