#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageBroadcastResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

   // Default player access check
    CPortalUser *thisAdmin = PORTAL_TABLE->get_by_portal_id(aPlayer->get_portal_id());
    CString error_msg;
    error_msg.format("%s '%s' %s.", 
                     GETTEXT("Access Level "), 
                     user_level_to_string(thisAdmin->get_user_level()), 
                     GETTEXT("does not grant access"));
    if (!ADMIN_TOOL->has_access(aPlayer, UL_ADMIN))
    {
       ITEM("ERROR_MESSAGE", (char *)error_msg);
       return output("admin/admin_error.html"); 
    }  

     // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));

	QUERY("BROADCAST_TYPE", BroadcastTypeString);
	if (BroadcastTypeString == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't specify how to broadcast a message."));

		return output("admin/admin_error.html");
	}

	if (strcmp(BroadcastTypeString, "MESSAGE") && strcmp(BroadcastTypeString, "MAIL"))
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You specified a wrong way to broadcast a message."));

		return output("admin/admin_error.html");
	}

	QUERY("BROADCAST_MESSAGE", BroadcastMessageString);
	if (BroadcastMessageString == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't input a broadcast message."));

		return output("admin/admin_error.html");
	}

	if (!strcmp(BroadcastTypeString, "MESSAGE"))
	{
		for (int i=0 ; i<PLAYER_TABLE->length() ; i++)
		{
			CPlayer *
				Player = (CPlayer *)PLAYER_TABLE->get(i);
			if (Player->get_game_id() == EMPIRE_GAME_ID) continue;

			CDiplomaticMessage *
				Message = new CDiplomaticMessage();
			Message->initialize(CDiplomaticMessage::TYPE_NORMAL, EMPIRE, Player, BroadcastMessageString);
			Player->add_diplomatic_message(Message);

			Message->CStore::type(QUERY_INSERT);
			STORE_CENTER->store(*Message);
		}
		ADMIN_TOOL->add_admin_action_log(thisAdmin, "Broadcasted a message to all players");
		ITEM("RESULT_MESSAGE", GETTEXT("You sent the broadcast message to all players."));
	}
	else if (!strcmp(BroadcastTypeString, "MAIL"))
	{
		ITEM("RESULT_MESSAGE", GETTEXT("This is not yet supported."));
	}

	ITEM("BROADCAST_MESSAGE", BroadcastMessageString);

//	system_log("end page handler %s", get_name());

	return output("admin/broadcast_result.html");
}
