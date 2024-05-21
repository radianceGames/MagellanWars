#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageCouncilDiplomaticMessageAccuse::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	CCouncil *
		Council = aPlayer->get_council();

	QUERY("MESSAGE_ID", MessageIDString);
	int
		MessageID = as_atoi(MessageIDString);
	CCouncilMessage *
		Message = Council->get_council_message(MessageID);
	if (Message == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Your council doesn't have a message with ID %1$s."),
								dec2unit(MessageID)));
		return output("council/diplomatic_message_accuse_error.html");
	}

	ADMIN_TOOL->accuse_message(Message);

	ITEM("MENU_TITLE", GETTEXT("Council Diplomatic Message - Accuse"));

	ITEM("RESULT_MESSAGE", GETTEXT("The message has been accused."));

//	system_log("end page handler %s", get_name());

	return output("council/diplomatic_message_accuse.html");
}
