#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerPunishmentDeleteCharacterResult::handler(CPlayer *aPlayer)
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

	CString
		PlayerNick;
	PlayerNick = Player->get_nick();

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
		Player->set_dead(
				(char *)format(GETTEXT("%1$s has been set dead by GM's punishment."),
								(char *)PlayerNick));
	    ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Killed %s", Player->get_nick()));
		ITEM("RESULT_MESSAGE",
				(char *)format(GETTEXT("%1$s has been set dead."), (char *)PlayerNick));

		ADMIN_TOOL->add_punishment_log(
				(char *)format("The player %s has been set dead by GM's punishment.",
								(char *)PlayerNick));

		return output("admin/player_punishment_delete_character_result.html");
	}
	else if (!strcmp(ByWhatString, "DELETE_THIS_PLAYER"))
	{
		Player->remove_from_database();
		Player->remove_from_memory();
		Player->remove_news_files();
		PLAYER_TABLE->remove_player(Player->get_game_id());
		ADMIN_TOOL->add_admin_action_log(thisAdmin, (char *)format("Deleted %s", Player->get_nick()));
		Player->set_dead(
				(char *)format(GETTEXT("%1$s has been deleted by GM's punishment."),
								(char *)PlayerNick));

		ITEM("RESULT_MESSAGE",
				(char *)format(GETTEXT("%1$s has been deleted."), (char *)PlayerNick));

		ADMIN_TOOL->add_punishment_log(
				(char *)format("The player %s has been deleted by GM's punishment.",
								(char *)PlayerNick));

		return output("admin/player_punishment_delete_character_result.html");
	}
	else if (!strcmp(ByWhatString, "DELETE_PORTAL_DATA"))
	{
		return output("admin/player_punishment_delete_character_result.html");
	}
	else
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You have selected a wrong command."));
		return output("admin/admin_error.html");
	}

	//	system_log("end page handler %s", get_name());
}

