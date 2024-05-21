#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerSendMessageResult::handler(CPlayer *aPlayer)
{

//	system_log("start page handler %s", get_name());

   // Default player access check
    CPortalUser *thisAdmin = PORTAL_TABLE->get_by_portal_id(aPlayer->get_portal_id());
    CString error_msg;
    error_msg.format("%s '%s' %s.", 
                     GETTEXT("Access Level "), 
                     user_level_to_string(thisAdmin->get_user_level()), 
                     GETTEXT("does not grant access"));
    if (!ADMIN_TOOL->has_access(aPlayer))
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

	QUERY("PORTAL_ID", PortalIDString);
	
	if (!PortalIDString)
	{
		ITEM("ERROR_MESSAGE",
			GETTEXT("You didn't specify a player ID."));
			return output("admin/admin_error.html");
	}
	else
	{
		int PortalID = as_atoi(PortalIDString);
		
		CPlayer *
			Player = PLAYER_TABLE->get_by_portal_id(PortalID);
		if (Player == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("The person whose portal ID is %s doesn't have a character."),
							dec2unit(0)));
			return output("admin/admin_error.html");
		}     
	    ITEM("PORTAL_ID", Player->get_portal_id());  
		QUERY("COMMENT", CommentString);
		if (CommentString == NULL)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You didn't input any comment."));
			return output("admin/admin_error.html");
		}

		CDiplomaticMessage *
			DiplomaticMessage = new CDiplomaticMessage();
		DiplomaticMessage->initialize(CDiplomaticMessage::TYPE_NORMAL, EMPIRE, Player, CommentString);
		Player->add_diplomatic_message(DiplomaticMessage);

		DiplomaticMessage->type(QUERY_INSERT);
		STORE_CENTER->store(*DiplomaticMessage);

		ITEM("RESULT_MESSAGE",
				(char *)format(GETTEXT("You sent a message to %s successfully."),
								Player->get_nick()));


	//	system_log("end page handler %s", get_name());

		return output("admin/player_send_message_result.html");
	}
}
