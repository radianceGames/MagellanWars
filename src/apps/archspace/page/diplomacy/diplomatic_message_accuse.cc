#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerDiplomaticMessageAccuse::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	QUERY("MESSAGE_ID", MessageIDString);
	int
		MessageID = as_atoi(MessageIDString);
	CDiplomaticMessage *
		Message = aPlayer->get_diplomatic_message(MessageID);
	if (Message == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("You don't have a message with ID %1$s."),
								dec2unit(MessageID)));
		return output("diplomacy/diplomatic_message_accuse_error.html");
	}

	ADMIN_TOOL->accuse_message(Message);

	ITEM("MENU_TITLE", GETTEXT("Diplomatic Message - Accuse"));

	ITEM("RESULT_MESSAGE", GETTEXT("The message has been accused."));

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/diplomatic_message_accuse.html" );
}

