#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerPunishmentDeleteCharacterConfirm::handler(CPlayer *aPlayer)
{

//	system_log( "start page handler %s", get_name());
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


    QUERY("PORTAL_ID",PortalIDString);
    int PortalID = 0;
    if (PortalIDString)
    {
       PortalID = as_atoi(PortalIDString);
    }

    ITEM("PORTAL_ID", PortalID);
	CPlayer *
		Player = PLAYER_TABLE->get_by_portal_id(PortalID);	if (!Player)
	if (!Player)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("This account doesn't have any characters."));
		return output("admin/admin_error.html");
	}

	QUERY("BY_WHAT", ByWhatString);
	if (!ByWhatString)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("By What String is NULL."
						" Maybe you should reload portal information again."));
		return output("admin/admin_error.html");
	}

	if (!strcmp(ByWhatString, "TO_MAKE_THIS_PLAYER_DEAD"))
	{
		ITEM("CONFIRM_MESSAGE",
				(char *)format(GETTEXT("Are you sure that you will make the player %1$s dead? You can't undo this operation."), Player->get_nick()));
		ITEM("BY_WHAT", "TO_MAKE_THIS_PLAYER_DEAD");
		return output("admin/player_punishment_delete_character_confirm.html");
	}
	else if (!strcmp(ByWhatString, "DELETE_THIS_PLAYER"))
	{
		ITEM("CONFIRM_MESSAGE",
				(char *)format(GETTEXT("Are you sure that you will delete the player %1$s? You can't undo this operation."), Player->get_nick()));
		ITEM("BY_WHAT", "DELETE_THIS_PLAYER");
		return output("admin/player_punishment_delete_character_confirm.html");
	}
	else if (!strcmp(ByWhatString, "DELETE_PORTAL_DATA"))
	{
		ITEM("CONFIRM_MESSAGE",
				(char *)format(GETTEXT("Are you sure that you will delete the player's portal data as well as the player's character? You can't undo this operation."), Player->get_nick()));
		ITEM("BY_WHAT", "DELETE_PORTAL_DATA");
		return output("admin/player_punishment_delete_character_confirm.html");
	}
	else
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You have selected a wrong command."));
		return output("admin/admin_error.html");
	}

	//	system_log("end page handler %s", get_name());
}

