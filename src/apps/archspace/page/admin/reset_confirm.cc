#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageResetConfirm::handler(CPlayer *aPlayer)
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


	QUERY("CONQUEROR_NAME", ConquerorNameString);
	if (ConquerorNameString == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't specify the conqueror's name."));
		return output("admin/admin_error.html");
	}

	QUERY("CONQUEROR_COUNCIL_NAME", ConquerorCouncilNameString);
	if (ConquerorCouncilNameString == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't specify a name of the conqueror's council."));
		return output("admin/admin_error.html");
	}

	ITEM("CONFIRM_MESSAGE",
			(char *)format(GETTEXT("Are you sure the server has to be reset? The Empire will be conquered by %1$s and the players will no more able to play the game."),
							ConquerorNameString));

	ITEM("CONQUEROR_NAME", ConquerorNameString);
	ITEM("CONQUEROR_COUNCIL_NAME", ConquerorCouncilNameString);

     // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));

//	system_log("end page handler %s", get_name());

	return output("admin/reset_confirm.html");
}
