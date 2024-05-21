#include <libintl.h>
#include "../../pages.h"

bool
CPageReadDiplomaticMessage::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	static CString
		MessageList;
	MessageList.clear();
	MessageList = aPlayer->get_message_list_html(0, false);

	if (MessageList.length() == 0)
	{
		CString HTML;

		HTML = "<TR>\n";
		HTML += "<TD CLASS=\"maintext\" ALIGN=CENTER COLSPAN=3>\n";
		HTML += GETTEXT("You have received no diplomatic messages.");
		HTML += "</TD>\n";
		HTML += "</TR>\n";

		ITEM("DIPLOMATIC_MESSAGE_LIST", (char *)HTML);
	}
	else
	{
		ITEM("DIPLOMATIC_MESSAGE_LIST", (char *)MessageList);
	}

	ITEM("STRING_UNANSWERED_MESSAGE",
			GETTEXT("View Unanswered Messages Only."));

	ITEM("STRING_SEARCH_BY_NAME", GETTEXT("Search by Name"));
	ITEM("STRING_SEARCH_BY_TYPE", GETTEXT("Search by Type"));

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/read_diplomatic_message.html" );
}

