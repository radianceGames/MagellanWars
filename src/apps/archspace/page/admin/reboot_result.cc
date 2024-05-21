#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageRebootResult::handler(CPlayer *aPlayer)
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

	QUERY("REBOOT", RebootString);
	if (RebootString == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You seem to have approached this page in a wrong way."));
		return output("admin/admin_error.html");
	}
	if (strcmp((char *)RebootString, "REBOOT"))
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You seem to have approached this page in a wrong way."));
		return output("admin/admin_error.html");
	}

	ADMIN_TOOL->add_admin_action_log(thisAdmin, "Restarted Archspace");
	CArchspace::shutdown_server();

	ITEM("RESULT_MESSAGE",
			(char *)format(GETTEXT("The server is rebooting in %s secs."),
							dec2unit(SERVER_SHUTDOWN_DELAY)));
    // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));

//	system_log("end page handler %s", get_name());

	return output("admin/reboot_result.html"); 
}
