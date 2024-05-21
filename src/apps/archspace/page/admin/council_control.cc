#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageCouncilControl::handler(CPlayer *aPlayer)
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
	static CString
		InputBoxPlayer,
		InputBoxPortal,
		InputBoxPortal1,
		InputBoxPortal2,
		InputBoxCouncil,
		InputBoxCouncil1,
		InputBoxCouncil2;
	InputBoxPlayer.clear();
	InputBoxPortal.clear();
	InputBoxPortal1.clear();
	InputBoxPortal2.clear();
	InputBoxCouncil.clear();
	InputBoxCouncil1.clear();
	InputBoxCouncil2.clear();

	InputBoxPlayer = "<INPUT TYPE=input SIZE=10 NAME=PLAYER_ID>";
	InputBoxPortal = "<INPUT TYPE=input SIZE=10 NAME=PORTAL_ID>";
	InputBoxPortal1 = "<INPUT TYPE=input SIZE=10 NAME=PORTAL_ID1>";
	InputBoxPortal2 = "<INPUT TYPE=input SIZE=10 NAME=PORTAL_ID2>";
	InputBoxCouncil = "<INPUT TYPE=input SIZE=10 NAME=COUNCIL_ID>";
	InputBoxCouncil1 = "<INPUT TYPE=input SIZE=10 NAME=COUNCIL_ID1>";
	InputBoxCouncil2 = "<INPUT TYPE=input SIZE=10 NAME=COUNCIL_ID2>";

	static CString
		Text;

	ITEM("STRING_DISBAND_COUNCIL_MOVE_COUNCIL_MEMBERS",
			GETTEXT("Disband Council/Move Council Members"));

	Text.clear();
	Text.format(GETTEXT("Disband council %1$s to random councils"),
				(char *)InputBoxCouncil);
	ITEM("DISBAND_TO_RANDOM_COUNCIL", (char *)Text);

	Text.clear();
	Text.format(GETTEXT("Transfer council member %1$s to council %2$s"),
				(char *)InputBoxPlayer,
				(char *)InputBoxCouncil);
	ITEM("TRANSFER_MEMBER_TO_CERTAIN_COUNCIL", (char *)Text);

	Text.clear();
	Text.format(GETTEXT("Transfer council member %1$s to random council"),
				(char *)InputBoxPlayer);
	ITEM("TRANSFER_MEMBER_TO_RANDOM_COUNCIL", (char *)Text);

	ITEM("STRING_RESTRICT_PLAYER", GETTEXT("Restrict Player"));

	Text.clear();
	Text.format(GETTEXT("Restrict player %1$s from same council as player %2$s"),
				(char *)InputBoxPortal1,
				(char *)InputBoxPortal2);
	ITEM("RESTRICT_PLAYER_FROM_PLAYER", (char *)Text);

	Text.clear();
	Text.format(GETTEXT("Restrict player %1$s from council %2$s"),
				(char *)InputBoxPortal,
				(char *)InputBoxCouncil);
	ITEM("RESTRICT_PLAYER_FROM_COUNCIL", (char *)Text);

	Text.clear();
	Text.format(GETTEXT("Search restrictions on player %1$s or portal ID %2$s"),
				(char *)InputBoxPortal,
				(char *)InputBoxPlayer);
	ITEM("SEARCH_RESTRICTIONS_ON_PLAYER", (char *)Text);

	ITEM("STRING_RESTRICT_COUNCIL", GETTEXT("Restrict Council"));

	Text.clear();
	Text.format(GETTEXT("Restrict council %1$s and council %2$s from submission"),
				(char *)InputBoxCouncil1,
				(char *)InputBoxCouncil2);
	ITEM("RESTRICT_COUNCILS_FROM_SUBMISSION", (char *)Text);

	Text.clear();
	Text.format(GETTEXT("Restrict council %1$s and council %2$s from merging"),
				(char *)InputBoxCouncil1,
				(char *)InputBoxCouncil2);
	ITEM("RESTRICT_COUNCILS_FROM_MERGING", (char *)Text);

	Text.clear();
	Text.format(GETTEXT("Restrict council %1$s and council %2$s from alliance"),
				(char *)InputBoxCouncil1,
				(char *)InputBoxCouncil2);
	ITEM("RESTRICT_COUNCILS_FROM_ALLIANCE", (char *)Text);

	Text.clear();
	Text.format(GETTEXT("Restrict council %1$s and council %2$s from war"),
				(char *)InputBoxCouncil1,
				(char *)InputBoxCouncil2);
	ITEM("RESTRICT_COUNCILS_FROM_WAR", (char *)Text);

	ITEM("STRING_TO_ADMIN_TOOL_MAIN", GETTEXT("To Admin Tool Main"));

      // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));


//	system_log("end page handler %s", get_name());

	return output("admin/council_control.html");
}
