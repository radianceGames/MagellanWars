#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageCouncilMessage::handler(CPlayer *aPlayer)
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

	QUERY("BY_WHAT", ByWhatString);
	if (!ByWhatString)
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

	if (!strcmp(ByWhatString, "COUNCIL_MAIL_RECEIVER_ID"))
	{
		QUERY("COUNCIL_MAIL_RECEIVER_ID", CouncilMailReceiverIDString);

		int
			CouncilID = as_atoi(CouncilMailReceiverIDString);
		CCouncil *
			Council = COUNCIL_TABLE->get_by_id(CouncilID);
		if (Council == NULL)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("That council doesn't exist."));
			return output("admin/council_message_error.html");
		}

		Title.format(GETTEXT("The messages received by the player %1$s"),
						Council->get_nick());

		List = Council->get_message_list_html(0, true);

		if (List.length() == 0)
		{
			List = "<TR>\n";
			List += "<TD CLASS=\"maintext\" ALIGN=CENTER COLSPAN=3>\n";
			List.format(GETTEXT("No council messages that the council %1$s has sent exist."),
						Council->get_nick());
			List += "</TD>\n";
			List += "</TR>\n";
		}
	}
	else if (!strcmp(ByWhatString, "COUNCIL_MAIL_SENDER_ID"))
	{
		QUERY("COUNCIL_MAIL_SENDER_ID", CouncilMailSenderIDString);

		int
			CouncilID = as_atoi(CouncilMailSenderIDString);
		CCouncil *
			Council = COUNCIL_TABLE->get_by_id(CouncilID);
		if (Council == NULL)
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("That council doesn't exist."));
			return output("admin/council_message_error.html");
		}

		Title.format(GETTEXT("The messages sent by the council %1$s"),
						Council->get_nick());

		CString
			List = CCouncil::get_message_list_by_sender_html(Council, true);

		if (List.length() == 0)
		{
			List = "<TR>\n";
			List += "<TD CLASS=\"maintext\" ALIGN=CENTER COLSPAN=3>\n";
			List.format(GETTEXT("No council messages that the council %1$s has sent exist."),
						Council->get_nick());
			List += "</TD>\n";
			List += "</TR>\n";
		}
	}
	else
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The way how to search a council was wrong."));
		return output("admin/admin_error.html");
	}

	ITEM("TITLE", (char *)Title);
	ITEM("MESSAGE_LIST", (char *)List);

//	system_log("end page handler %s", get_name());

	return output("admin/council_message.html");
}

