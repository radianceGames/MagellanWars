#include <libintl.h>
#include "../../pages.h"

bool
CPageCouncilSendDiplomaticMessage::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	ITEM("MENU_TITLE", GETTEXT("Council Diplomacy - Send Message"));

	CCouncil *
		Council = aPlayer->get_council();

	CString
		CouncilHTML = Council->select_council_html();

	if (CouncilHTML.length() == 0)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There's no council that you can send a message to."));
		return output("council/send_diplomatic_message_error.html");
	}

	ITEM("SELECT_COUNCIL_MESSAGE",
			GETTEXT("Select the name of the council you want to send a message."));

	ITEM("COUNCIL_LIST", (char *)CouncilHTML);

	ITEM("ENTER_NUMBER_MESSAGE",
			GETTEXT("Or enter the cluster number and the ID of the council you want to send a message to."));

	ITEM("STRING_COUNCIL__", GETTEXT("Council #"));

//	system_log("end page handler %s", get_name());

	return output("council/send_diplomatic_message.html");
}

