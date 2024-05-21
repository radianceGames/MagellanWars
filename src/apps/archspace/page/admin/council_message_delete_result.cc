#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageCouncilMessageDeleteResult::handler(CPlayer *aPlayer)
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

	QUERY("COUNCIL_ID", CouncilIDString);
	int
		CouncilID = as_atoi(CouncilIDString);
	CCouncil *
		Council = COUNCIL_TABLE->get_by_id(CouncilID);
	if (Council == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no such a council with ID #%1$s."),
								dec2unit(CouncilID)));
		return output("admin/admin_error.html");

	}

	QUERY("MESSAGE_ID", MessageIDString);
	int
		MessageID = as_atoi(MessageIDString);

	CCouncilMessageBox *
		MessageBox = Council->get_council_message_box();
	CCouncilMessage *
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
	MessageBox->remove_council_message(MessageID);

	ITEM("RESULT_MESSAGE",
			(char *)format(GETTEXT("The message(#%1$d) has been removed successfully."),
							MessageID));

//	system_log("end page handler %s", get_name());

	return output("admin/player_message_delete_result.html");
}

