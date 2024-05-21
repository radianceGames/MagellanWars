#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageCouncilMessageRead::handler(CPlayer *aPlayer)
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

	QUERY("COUNCIL_ID", CouncilIDString);
	int
		CouncilID = as_atoi(CouncilIDString);
	CCouncil *
		Council = COUNCIL_TABLE->get_by_id(CouncilID);
	if (Council == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no such a council with ID #%1$s."),
								dec2unit(CouncilID)));
		return output("admin/admin_error.html");
	}

	QUERY("MESSAGE_ID", MessageIDString);
	int
		MessageID = as_atoi(MessageIDString);

	CCouncilMessageBox *
		MessageBox = Council->get_council_message_box();
	CCouncilMessage *
		Message = MessageBox->get_by_id(MessageID);

	if (Message == NULL)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("This council doesn't have that message(#%1$d)."),
								MessageID));
		return output("admin/admin_error.html");
	}

	ITEM("STRING_NO_", GETTEXT("No."));
	ITEM("STRING_SENDER", GETTEXT("Sender"));
	ITEM("STRING_DATE", GETTEXT("Date"));
	ITEM("STRING_MESSAGE_TYPE", GETTEXT("Message Type"));
	ITEM("STRING_REPLY_STATUS", GETTEXT("Reply Status"));
	ITEM("STRING_COMMENT", GETTEXT("Comment"));

	ITEM("NO_", Message->get_id());
	if (Message->get_sender() == NULL)
	{
		ITEM("FROM", GETTEXT("N/A"));
	}
	else
	{
		ITEM("FROM", Message->get_sender()->get_nick());
	}
	ITEM("DATE", Message->get_time_string());
	ITEM("MESSAGE_TYPE", Message->get_type_description());
	ITEM("REPLY_STATUS", Message->get_status_description());

	CString
		Comment;
	Comment = Message->get_content();
	Comment.htmlspecialchars();
	Comment.nl2br();
	ITEM("COMMENT", (char *)Comment);

//	system_log("end page handler %s", get_name());

	return output("admin/council_message_read.html");
}

