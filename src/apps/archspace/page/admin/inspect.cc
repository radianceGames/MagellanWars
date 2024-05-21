#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageInspect::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

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

	ITEM("PLAYER_SELECT_MESSAGE",
			GETTEXT("Please enter a name of the player who you want to investigate."));

	ITEM("STRING_ACCOUNT_ID", GETTEXT("Search by: Account ID"));
	ITEM("STRING_ACCOUNT_NAME", GETTEXT("Search by: Account Name"));
	ITEM("STRING_PLAYER_ID", GETTEXT("Search by: Player ID"));
	ITEM("STRING_PLAYER_NAME", GETTEXT("Search by: Player Name"));
	ITEM("STRING_PLAYER_MAIL_RECEIVER_ID", GETTEXT("Search by: Player Mail Receiver ID"));
	ITEM("STRING_PLAYER_MAIL_SENDER_ID", GETTEXT("Search by: Player Mail Sender ID"));

	ITEM("STRING_COUNCIL_ID", GETTEXT("Search by: Council ID"));
	ITEM("STRING_COUNCIL_NAME", GETTEXT("Search by: Council Name"));
	ITEM("STRING_COUNCIL_MAIL_RECEIVER_ID", GETTEXT("Search by: Council Mail Receiver ID"));
	ITEM("STRING_COUNCIL_MAIL_SENDER_ID", GETTEXT("Search by: Council Mail Sender ID"));

	ITEM("STRING_ACTION_LOG", GETTEXT("Action Log"));
	ITEM("STRING_NEW_ACCOUNT_LOG", GETTEXT("New Account Log"));
	ITEM("STRING_NEW_PLAYER_LOG", GETTEXT("New Player Log"));
	ITEM("STRING_DEAD_PLAYER_LOG", GETTEXT("Dead Player Log"));

	ITEM("STRING_SEE_LOG_MESSAGE",
			GETTEXT("To search any kind of action, choose a kind and date."));

	ITEM("STRING_YEAR", GETTEXT("Year"));
	ITEM("STRING_MONTH", GETTEXT("Month"));
	ITEM("STRING_DAY", GETTEXT("Day"));

     // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));

//	system_log("end page handler %s", get_name());

	return output("admin/inspect.html");
}
