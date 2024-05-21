#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageBlackMarketControlResult::handler(CPlayer *aPlayer)
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
	static CString
		Result;
	Result.clear();
 

	QUERY("BY_WHAT", ByWhatString);
	if (ByWhatString == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("ByWhatString is NULL."));

		return output("admin/blackmarket_control_error.html");
	}

	if (!strcmp(ByWhatString, "CREATE_NEW_TECH"))
	{
		BLACK_MARKET->create_new_tech();
		ADMIN_TOOL->add_admin_action_log(thisAdmin, "Added a tech to the black market");		
		ITEM("RESULT_MESSAGE", GETTEXT("A new tech has been created."));
	}
	else if (!strcmp(ByWhatString, "CREATE_NEW_FLEET"))
	{
		BLACK_MARKET->create_new_fleet();
		ADMIN_TOOL->add_admin_action_log(thisAdmin, "Added a fleet to the black market");		
		ITEM("RESULT_MESSAGE", GETTEXT("A new fleet has been created."));
	}
	else if (!strcmp(ByWhatString, "CREATE_NEW_ADMIRAL"))
	{
		BLACK_MARKET->create_new_admiral();
		ADMIN_TOOL->add_admin_action_log(thisAdmin, "Added an admiral to the black market");		
		ITEM("RESULT_MESSAGE", GETTEXT("A new admiral has been created."));
	}
	else if (!strcmp(ByWhatString, "CREATE_NEW_PROJECT"))
	{
		BLACK_MARKET->create_new_project();
		ADMIN_TOOL->add_admin_action_log(thisAdmin, "Added a project to the black market");		
		ITEM("RESULT_MESSAGE", GETTEXT("A new project has been created."));
	}
	else if (!strcmp(ByWhatString, "CREATE_NEW_PLANET"))
	{
		BLACK_MARKET->create_new_planet();
		ADMIN_TOOL->add_admin_action_log(thisAdmin, "Added a planet to the black market");
		ITEM("RESULT_MESSAGE", GETTEXT("A new planet has been created."));
	}
	else if (!strcmp(ByWhatString, "CLEAR_BLACK_MARKET"))
	{
        BLACK_MARKET->expire_all();
        ADMIN_TOOL->add_admin_action_log(thisAdmin, "Cleared the Black Market");
		ITEM("RESULT_MESSAGE", GETTEXT("The Black Market has been cleared."));
	}
	else
	{
		ITEM("ERROR_MESSAGE", GETTEXT("ByWhatString seems wrong. Please try again."));
		return output("admin/blackmarket_control_error.html");
	}

      // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));


//	system_log("end page handler %s", get_name());

	return output("admin/black_market_control_result.html");
}
