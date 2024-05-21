#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageBlackMarketControl::handler(CPlayer *aPlayer)
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
    ITEM("STRING_CREATE_NEW_TECH", GETTEXT("Create New Tech"));
	ITEM("STRING_CREATE_NEW_FLEET", GETTEXT("Create New Fleet"));
	ITEM("STRING_CREATE_NEW_ADMIRAL", GETTEXT("Create New Admiral"));
	ITEM("STRING_CREATE_NEW_PROJECT", GETTEXT("Create New Project"));
	ITEM("STRING_CREATE_NEW_PLANET", GETTEXT("Create New Planet"));
	ITEM("STRING_CLEAR_BLACK_MARKET", GETTEXT("Clear Black Market"));	

	ITEM("STRING_TO_ADMIN_TOOL_MAIN", GETTEXT("To Admin Tool Main"));
     // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));


//	system_log("end page handler %s", get_name());

	return output("admin/black_market_control.html");
}
