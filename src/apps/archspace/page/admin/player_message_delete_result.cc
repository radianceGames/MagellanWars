#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerMessageDeleteResult::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

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

	QUERY("PLAYER_GAME_ID", PlayerGameIDString);
	int
		PlayerGameID = as_atoi(PlayerGameIDString);
	CPlayer *
		Player = PLAYER_TABLE->get_by_game_id(PlayerGameID);
	if (Player == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no such a player with ID #%1$s."),
								dec2unit(PlayerGameID)));
		return output("admin/admin_error.html");

	}
    ITEM("PORTAL_ID", Player->get_portal_id());  
	QUERY("MESSAGE_ID", MessageIDString);
	int
		MessageID = as_atoi(MessageIDString);

	CDiplomaticMessageBox *
		MessageBox = Player->get_diplomatic_message_box();
	CDiplomaticMessage *
		Message = MessageBox->get_by_id(MessageID);

	if (Message == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("This player doesn't have that message(#%1$d)."),
								MessageID));
		return output("admin/admin_error.html");
	}

	Message->type(QUERY_DELETE);
	STORE_CENTER->store(*Message);
	MessageBox->remove_diplomatic_message(MessageID);

	ITEM("RESULT_MESSAGE",
			(char *)format(GETTEXT("The message(#%1$d) has been removed successfully."),
							MessageID));

//	system_log("end page handler %s", get_name());

	return output("admin/player_message_delete_result.html");
}

