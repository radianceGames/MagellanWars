#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerMessage::handler(CPlayer *aPlayer)
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

    QUERY("PORTAL_ID", PortalIDString);
	QUERY("BY_WHAT", ByWhatString);
	if (!ByWhatString && !PortalIDString)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You didn't specify the way how to search messages."));
		return output("admin/admin_error.html");
	}
    
	static CString
		Title,
		List;
	Title.clear();
	List.clear();

	if (ByWhatString && !strcmp(ByWhatString, "PLAYER_MAIL_RECEIVER_ID"))
	{
		QUERY("PLAYER_MAIL_RECEIVER_ID", PlayerMailReceiverIDString);

		int
			PlayerID = as_atoi(PlayerMailReceiverIDString);
		CPlayer *
			Player = PLAYER_TABLE->get_by_game_id(PlayerID);
		if (Player == NULL)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("That player doesn't exist."));
			return output("admin/admin_error.html");
		}
        ITEM("PORTAL_ID", Player->get_portal_id());  
		Title.format(GETTEXT("The messages received by the player %1$s"),
						Player->get_nick());

		List = Player->get_message_list_html(0, true);

		if (List.length() == 0)
		{
			List = "<TR>\n";
			List += "<TD CLASS=\"maintext\" ALIGN=CENTER COLSPAN=3>\n";
			List.format(GETTEXT("No diplomatic messages that the player %1$s has sent exist."),
						Player->get_nick());
			List += "</TD>\n";
			List += "</TR>\n";
		}
	}
	else if (ByWhatString && !strcmp(ByWhatString, "PLAYER_MAIL_SENDER_ID"))
	{
		QUERY("PLAYER_MAIL_SENDER_ID", PlayerMailSenderIDString);

		int
			PlayerID = as_atoi(PlayerMailSenderIDString);
		CPlayer *
			Player = PLAYER_TABLE->get_by_game_id(PlayerID);
		if (Player == NULL)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("That player doesn't exist."));
			return output("admin/admin_error.html");
		}
        ITEM("PORTAL_ID", Player->get_portal_id());
		Title.format(GETTEXT("The messages sent by the player %1$s"),
						Player->get_nick());

		CString
			List = CPlayer::get_message_list_by_sender_html(Player, true);

		if (List.length() == 0)
		{
			List = "<TR>\n";
			List += "<TD CLASS=\"maintext\" ALIGN=CENTER COLSPAN=3>\n";
			List.format(GETTEXT("No diplomatic messages that the player %1$s has sent exist."),
						Player->get_nick());
			List += "</TD>\n";
			List += "</TR>\n";
		}
	} 
	else if (PortalIDString)
    {
      int PortalID = as_atoi(PortalIDString);
		CPlayer *
			Player = PLAYER_TABLE->get_by_portal_id(PortalID);
		if (Player == NULL)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("That player doesn't exist."));
			return output("admin/admin_error.html");
		}
        ITEM("PORTAL_ID", Player->get_portal_id());  
		Title.format(GETTEXT("The messages received by the player %1$s"),
						Player->get_nick());

		List = Player->get_message_list_html(0, true);

		if (List.length() == 0)
		{
			List = "<TR>\n";
			List += "<TD CLASS=\"maintext\" ALIGN=CENTER COLSPAN=3>\n";
			List.format(GETTEXT("No diplomatic messages that the player %1$s has sent exist."),
						Player->get_nick());
			List += "</TD>\n";
			List += "</TR>\n";
		}

    }
    else
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The way how to search a player was wrong."));
		return output("admin/admin_error.html");
	}

	ITEM("TITLE", (char *)Title);
	ITEM("MESSAGE_LIST", (char *)List);

//	system_log("end page handler %s", get_name());

	return output("admin/player_message.html");
}

