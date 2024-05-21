#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerConfirm::handler(CPlayer *aPlayer)
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
    
    QUERY("PORTAL_ID", PortalIDString);
	QUERY("BY_WHAT", ByWhatString);
	if (!ByWhatString && !PortalIDString)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You didn't specify the way how to search for a player."));
		return output("admin/admin_error.html");
	}

	CPlayer *
		Player = NULL;
    CPortalUser *PortalUser = NULL;
	if (!strcmp(ByWhatString, "PLAYER_ID"))
	{
		QUERY("PLAYER_ID", PlayerIDString);

		int
			PlayerID = as_atoi(PlayerIDString);
		Player = PLAYER_TABLE->get_by_game_id(PlayerID);
	}
	else if (!strcmp(ByWhatString, "PLAYER_NAME"))
	{
		QUERY("PLAYER_NAME", PlayerNameString);

		if (!PlayerNameString)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You didn't enter a player name."));
			return output("admin/admin_error.html");
		}

		Player = PLAYER_TABLE->get_by_name(PlayerNameString);
	}
	else if (!strcmp(ByWhatString, "PORTAL_ID"))
	{
		QUERY("PORTAL_ID", PortalIDString);
        
        int PortalID = as_atoi(PortalIDString);
		PortalUser = PORTAL_TABLE->get_by_portal_id(PortalID);
	}
	else if (!strcmp(ByWhatString, "PORTAL_NAME"))
	{
		QUERY("PORTAL_NAME", PortalNameString);

		if (!PortalNameString)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("You didn't enter a portal name."));
			return output("admin/admin_error.html");
		}
        // need to add a method for this to PORTAL_TABLE  
		for (int i=0; i < PORTAL_TABLE->length(); i++)
        {
           if (!strcmp(((CPortalUser*)PORTAL_TABLE->get(i))->get_name(), PortalNameString))
              PortalUser = (CPortalUser *)PORTAL_TABLE->get(i);
        }
	}
	else
	{
		ITEM("ERROR_MESSAGE", GETTEXT("The specified player search method is invalid."));
		return output("admin/admin_error.html");
	}
    
    CString ConfirmMessage;
    
	if (Player == NULL && PortalUser == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("No results found."));
		return output("admin/admin_error.html");
	}
	else if (Player != NULL)
    {
     if (Player->get_game_id() == EMPIRE_GAME_ID)
      {
		ITEM("ERROR_MESSAGE", GETTEXT("You can't inspect the Empire."));
		return output("admin/admin_error.html");
      }
     else
      {
         ConfirmMessage.format("%s%s%s", GETTEXT("Are you sure this player: "),
							Player->get_nick(),
                            GETTEXT("?"));
         ITEM("PORTAL_ID", Player->get_portal_id());
      }
	}
    else if (PortalUser != NULL)
    {
         ConfirmMessage.format("%s(#%d)%s%s", GETTEXT("Are you sure this user: "),
							PortalUser->get_portal_id(),
							PortalUser->get_name(),       
                            GETTEXT("?"));
         ITEM("PORTAL_ID", PortalUser->get_portal_id());
    }
    ITEM("CONFIRM_MESSAGE",(char *)ConfirmMessage);

//	system_log("end page handler %s", get_name());
     // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));


	return output("admin/player_confirm.html");
}
