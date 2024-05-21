#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerPunishment::handler(CPlayer *aPlayer)
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


    QUERY("PORTAL_ID",PortalIDString);
    int PortalID = 0;
    if (PortalIDString)
    {
       PortalID = as_atoi(PortalIDString);
    }

    ITEM("PORTAL_ID", PortalID);
	CPlayer *
		Player = PLAYER_TABLE->get_by_portal_id(PortalID);	if (!Player)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("This account doesn't have any characters."));
		return output("admin/admin_error.html");

	}

	ITEM("STRING_IMPERIAL_RETRIBUTION", GETTEXT("Imperial Retribution"));
	ITEM("STRING_REMOVE_IMPERIAL_RETRIBUTION", GETTEXT("Remove Imperial Retribution"));

	ITEM("STRING_SKIP_TURN_PUNISHMENT", GETTEXT("Skip Turn Punishment"));
	ITEM("STRING_TURN_S_", GETTEXT("Turn(s)"));

	ITEM("STRING_TO_MAKE_THIS_PLAYER_DEAD", GETTEXT("To Make This Player Dead"));
	ITEM("STRING_DELETE_THIS_PLAYER", GETTEXT("Delete This Player"));
	ITEM("STRING_DELETE_PORTAL_DATA", GETTEXT("Delete Portal Data"));

//	system_log("end page handler %s", get_name());

	return output("admin/player_punishment.html");
}

