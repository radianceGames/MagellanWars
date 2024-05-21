#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageCouncilControlResult::handler(CPlayer *aPlayer)
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

         // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));


	static CString
		Result;
	Result.clear();

	QUERY("BY_WHAT", ByWhatString);
	if (ByWhatString == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("ByWhatString is NULL."));

		return output("admin/council_control_error.html");
	}

	if (!strcmp(ByWhatString, "DISBAND_TO_RANDOM_COUNCIL"))
	{
		QUERY("COUNCIL_ID", CouncilIDString);

		int
			CouncilID = as_atoi(CouncilIDString);

		CCouncil *
			Council = COUNCIL_TABLE->get_by_id(CouncilID);

		if (Council == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no council with ID %1$s."),
									dec2unit(CouncilID)));
			return output("admin/council_control_error.html");
		}
	}
	else if (!strcmp(ByWhatString, "TRANSFER_MEMBER_TO_CERTAIN_COUNCIL"))
	{
	}
	else if (!strcmp(ByWhatString, "TRANSFER_MEMBER_TO_RANDOM_COUNCIL"))
	{
	}
	else if (!strcmp(ByWhatString, "RESTRICT_PLAYER_FROM_PLAYER"))
	{
		QUERY("PORTAL_ID1", PortalID1String);
		QUERY("PORTAL_ID2", PortalID2String);

		int
			PortalID1 = as_atoi(PortalID1String),
			PortalID2 = as_atoi(PortalID2String);

		CPlayer
			*Portal1 = PLAYER_TABLE->get_by_portal_id(PortalID1),
			*Portal2 = PLAYER_TABLE->get_by_portal_id(PortalID2);

		if (Portal1 == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no player with portal ID %1$s."),
									dec2unit(PortalID1)));
			return output("admin/council_control_error.html");
		}

		if (Portal2 == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no player with portal ID %1$s."),
									dec2unit(PortalID2)));
			return output("admin/council_control_error.html");
		}

		if (Portal1->get_portal_id() == Portal2->get_portal_id())
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("2 players have the same portal ID."));
			return output("admin/council_control_error.html");
		}

		CDetachmentPlayerPlayer *
			DetachmentPlayerPlayer = DETACHMENT_PLAYER_PLAYER_TABLE->get_by_player_player(Portal1, Portal2);
		if (DetachmentPlayerPlayer == NULL)
		{
			CDetachmentPlayerPlayer *
				NewDetachmentPlayerPlayer = new CDetachmentPlayerPlayer();
			NewDetachmentPlayerPlayer->set_id(DETACHMENT_PLAYER_PLAYER_TABLE->get_new_id());
			NewDetachmentPlayerPlayer->set_player1(Portal1);
			NewDetachmentPlayerPlayer->set_player2(Portal2);

			DETACHMENT_PLAYER_PLAYER_TABLE->add_detachment(NewDetachmentPlayerPlayer);

			NewDetachmentPlayerPlayer->type(QUERY_INSERT);
			STORE_CENTER->store(*NewDetachmentPlayerPlayer);

			ITEM("RESULT_MESSAGE", GETTEXT("They can't be in the same council anymore."));

			return output("admin/council_control_result.html");
		}
		else
		{
			DetachmentPlayerPlayer->type(QUERY_DELETE);
			STORE_CENTER->store(*DetachmentPlayerPlayer);

			DETACHMENT_PLAYER_PLAYER_TABLE->remove_detachment(DetachmentPlayerPlayer->get_id());

			ITEM("RESULT_MESSAGE", GETTEXT("They can be in the same council again."));

			return output("admin/council_control_result.html");
		}
	}
	else if (!strcmp(ByWhatString, "RESTRICT_PLAYER_FROM_COUNCIL"))
	{
		QUERY("PORTAL_ID", PortalIDString);
		QUERY("COUNCIL_ID", CouncilIDString);

		int
			PortalID = as_atoi(PortalIDString),
			CouncilID = as_atoi(CouncilIDString);

		CPlayer *
			Portal = PLAYER_TABLE->get_by_portal_id(PortalID);
		CCouncil *
			Council = COUNCIL_TABLE->get_by_id(CouncilID);

		if (Portal == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no player with portal ID %1$s."),
									dec2unit(PortalID)));
			return output("admin/council_control_error.html");
		}

		if (Council == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no council with ID %1$s."),
									dec2unit(CouncilID)));
			return output("admin/council_control_error.html");
		}

		CDetachmentPlayerCouncil *
			DetachmentPlayerCouncil = DETACHMENT_PLAYER_COUNCIL_TABLE->get_by_player_council(Portal, Council);
		if (DetachmentPlayerCouncil == NULL)
		{
			CDetachmentPlayerCouncil *
				NewDetachmentPlayerCouncil = new CDetachmentPlayerCouncil();
			NewDetachmentPlayerCouncil->set_id(DETACHMENT_PLAYER_COUNCIL_TABLE->get_new_id());
			NewDetachmentPlayerCouncil->set_player(Portal);
			NewDetachmentPlayerCouncil->set_council(Council);

			DETACHMENT_PLAYER_COUNCIL_TABLE->add_detachment(NewDetachmentPlayerCouncil);

			NewDetachmentPlayerCouncil->type(QUERY_INSERT);
			STORE_CENTER->store(*NewDetachmentPlayerCouncil);

			ITEM("RESULT_MESSAGE",
					(char *)format(GETTEXT("This player can't join the council %1$s anymore."),
									Council->get_nick()));

			return output("admin/council_control_result.html");
		}
		else
		{
			DetachmentPlayerCouncil->type(QUERY_DELETE);
			STORE_CENTER->store(*DetachmentPlayerCouncil);

			DETACHMENT_PLAYER_COUNCIL_TABLE->remove_detachment(DetachmentPlayerCouncil->get_id());

			ITEM("RESULT_MESSAGE",
					(char *)format(GETTEXT("This player can join the council %1$s again."),
									Council->get_nick()));

			return output("admin/council_control_result.html");
		}
	}
	else if (!strcmp(ByWhatString, "SEARCH_RESTRICTIONS_ON_PLAYER"))
	{
		QUERY("PLAYER_ID", PlayerIDString);
		QUERY("PORTAL_ID", PortalIDString);

		int
			PlayerID = as_atoi(PlayerIDString),
			PortalID = as_atoi(PortalIDString);

		CPlayer *
			Player = NULL;
		if (PlayerID > 0)
		{
			Player = PLAYER_TABLE->get_by_game_id(PlayerID);
		}
		else if (PortalID > 0)
		{
			Player = PLAYER_TABLE->get_by_portal_id(PortalID);
		}
		else
		{
			ITEM("ERROR_MESSAGE", GETTEXT("That player doesn't exist."));
			return output("admin/council_control_error.html");
		}

		Result = "<TABLE BORDER=0>\n";

		Result += "<TR>\n";
		Result.format("<TH CLASS=\"maintext\" ALIGN=\"center\">%1$s</TH>\n",
						GETTEXT("Restriction From a Player"));
		Result += "</TR>\n";

		Result += "<TR>\n";
		Result += "<TD>&nbsp;</TD>\n";
		Result += "</TR>\n";

		Result += DETACHMENT_PLAYER_PLAYER_TABLE->get_restricted_player_list_html(Player);

		Result += "<TR>\n";
		Result += "<TD>&nbsp;</TD>\n";
		Result += "</TR>\n";

		Result += "<TR>\n";
		Result.format("<TH CLASS=\"maintext\" ALIGN=\"center\">%1$s</TH>\n",
						GETTEXT("Restriction From a Council"));
		Result += "</TR>\n";

		Result += "<TR>\n";
		Result += "<TD>&nbsp;</TD>\n";
		Result += "</TR>\n";

		Result += DETACHMENT_PLAYER_COUNCIL_TABLE->get_restricted_council_list_html(Player);

		Result += "</TABLE>\n";

		ITEM("RESULT_MESSAGE", (char *)Result);

		return output("admin/council_control_result.html");
	}
	else if (!strcmp(ByWhatString, "RESTRICT_COUNCILS_FROM_SUBMISSION"))
	{
		QUERY("COUNCIL_ID1", CouncilID1String);
		QUERY("COUNCIL_ID2", CouncilID2String);

		int
			CouncilID1 = as_atoi(CouncilID1String),
			CouncilID2 = as_atoi(CouncilID2String);

		CCouncil
			*Council1 = COUNCIL_TABLE->get_by_id(CouncilID1),
			*Council2 = COUNCIL_TABLE->get_by_id(CouncilID2);

		if (Council1 == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no council with ID %1$s."),
									dec2unit(CouncilID1)));
			return output("admin/council_control_error.html");
		}

		if (Council2 == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no council with ID %1$s."),
									dec2unit(CouncilID2)));
			return output("admin/council_control_error.html");
		}

		if (Council1->get_id() == Council2->get_id())
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("2 councils have the same ID."));
			return output("admin/council_control_error.html");
		}

		CDetachmentCouncilCouncil *
			DetachmentCouncilCouncil = DETACHMENT_COUNCIL_COUNCIL_TABLE->get_by_council_council(Council1, Council2);
		if (DetachmentCouncilCouncil == NULL)
		{
			CDetachmentCouncilCouncil *
				NewDetachmentCouncilCouncil = new CDetachmentCouncilCouncil();
			NewDetachmentCouncilCouncil->set_id(DETACHMENT_COUNCIL_COUNCIL_TABLE->get_new_id());
			NewDetachmentCouncilCouncil->set_type(CDetachmentCouncilCouncil::TYPE_SUBMISSION);
			NewDetachmentCouncilCouncil->set_council1(Council1);
			NewDetachmentCouncilCouncil->set_council2(Council2);

			DETACHMENT_COUNCIL_COUNCIL_TABLE->add_detachment(NewDetachmentCouncilCouncil);

			NewDetachmentCouncilCouncil->type(QUERY_INSERT);
			STORE_CENTER->store(*NewDetachmentCouncilCouncil);

			ITEM("RESULT_MESSAGE",
					GETTEXT("The councils have restriction from submission now."));

			return output("admin/council_control_result.html");
		}
		else
		{
			DetachmentCouncilCouncil->type(QUERY_DELETE);
			STORE_CENTER->store(*DetachmentCouncilCouncil);

			DETACHMENT_COUNCIL_COUNCIL_TABLE->remove_detachment(DetachmentCouncilCouncil->get_id());

			ITEM("RESULT_MESSAGE",
					GETTEXT("The councils don't have restriction from submission now."));

			return output("admin/council_control_result.html");
		}
	}
	else if (!strcmp(ByWhatString, "RESTRICT_COUNCILS_FROM_MERGING"))
	{
		QUERY("COUNCIL_ID1", CouncilID1String);
		QUERY("COUNCIL_ID2", CouncilID2String);

		int
			CouncilID1 = as_atoi(CouncilID1String),
			CouncilID2 = as_atoi(CouncilID2String);

		CCouncil
			*Council1 = COUNCIL_TABLE->get_by_id(CouncilID1),
			*Council2 = COUNCIL_TABLE->get_by_id(CouncilID2);

		if (Council1 == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no council with ID %1$s."),
									dec2unit(CouncilID1)));
			return output("admin/council_control_error.html");
		}

		if (Council2 == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no council with ID %1$s."),
									dec2unit(CouncilID2)));
			return output("admin/council_control_error.html");
		}

		if (Council1->get_id() == Council2->get_id())
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("2 councils have the same ID."));
			return output("admin/council_control_error.html");
		}

		CDetachmentCouncilCouncil *
			DetachmentCouncilCouncil = DETACHMENT_COUNCIL_COUNCIL_TABLE->get_by_council_council(Council1, Council2);
		if (DetachmentCouncilCouncil == NULL)
		{
			CDetachmentCouncilCouncil *
				NewDetachmentCouncilCouncil = new CDetachmentCouncilCouncil();
			NewDetachmentCouncilCouncil->set_id(DETACHMENT_COUNCIL_COUNCIL_TABLE->get_new_id());
			NewDetachmentCouncilCouncil->set_type(CDetachmentCouncilCouncil::TYPE_MERGING);
			NewDetachmentCouncilCouncil->set_council1(Council1);
			NewDetachmentCouncilCouncil->set_council2(Council2);

			DETACHMENT_COUNCIL_COUNCIL_TABLE->add_detachment(NewDetachmentCouncilCouncil);

			NewDetachmentCouncilCouncil->type(QUERY_INSERT);
			STORE_CENTER->store(*NewDetachmentCouncilCouncil);

			ITEM("RESULT_MESSAGE",
					GETTEXT("The councils have restriction from merging now."));

			return output("admin/council_control_result.html");
		}
		else
		{
			DetachmentCouncilCouncil->type(QUERY_DELETE);
			STORE_CENTER->store(*DetachmentCouncilCouncil);

			DETACHMENT_COUNCIL_COUNCIL_TABLE->remove_detachment(DetachmentCouncilCouncil->get_id());

			ITEM("RESULT_MESSAGE",
					GETTEXT("The councils don't have restriction from merging now."));

			return output("admin/council_control_result.html");
		}
	}
	else if (!strcmp(ByWhatString, "RESTRICT_COUNCILS_FROM_ALLIANCE"))
	{
		QUERY("COUNCIL_ID1", CouncilID1String);
		QUERY("COUNCIL_ID2", CouncilID2String);

		int
			CouncilID1 = as_atoi(CouncilID1String),
			CouncilID2 = as_atoi(CouncilID2String);

		CCouncil
			*Council1 = COUNCIL_TABLE->get_by_id(CouncilID1),
			*Council2 = COUNCIL_TABLE->get_by_id(CouncilID2);

		if (Council1 == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no council with ID %1$s."),
									dec2unit(CouncilID1)));
			return output("admin/council_control_error.html");
		}

		if (Council2 == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no council with ID %1$s."),
									dec2unit(CouncilID2)));
			return output("admin/council_control_error.html");
		}

		if (Council1->get_id() == Council2->get_id())
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("2 councils have the same ID."));
			return output("admin/council_control_error.html");
		}

		CDetachmentCouncilCouncil *
			DetachmentCouncilCouncil = DETACHMENT_COUNCIL_COUNCIL_TABLE->get_by_council_council(Council1, Council2);
		if (DetachmentCouncilCouncil == NULL)
		{
			CDetachmentCouncilCouncil *
				NewDetachmentCouncilCouncil = new CDetachmentCouncilCouncil();
			NewDetachmentCouncilCouncil->set_id(DETACHMENT_COUNCIL_COUNCIL_TABLE->get_new_id());
			NewDetachmentCouncilCouncil->set_type(CDetachmentCouncilCouncil::TYPE_ALLIANCE);
			NewDetachmentCouncilCouncil->set_council1(Council1);
			NewDetachmentCouncilCouncil->set_council2(Council2);

			DETACHMENT_COUNCIL_COUNCIL_TABLE->add_detachment(NewDetachmentCouncilCouncil);

			NewDetachmentCouncilCouncil->type(QUERY_INSERT);
			STORE_CENTER->store(*NewDetachmentCouncilCouncil);

			ITEM("RESULT_MESSAGE",
					GETTEXT("The councils have restriction from alliance now."));

			return output("admin/council_control_result.html");
		}
		else
		{
			DetachmentCouncilCouncil->type(QUERY_DELETE);
			STORE_CENTER->store(*DetachmentCouncilCouncil);

			DETACHMENT_COUNCIL_COUNCIL_TABLE->remove_detachment(DetachmentCouncilCouncil->get_id());

			ITEM("RESULT_MESSAGE",
					GETTEXT("The councils don't have restriction from alliance now."));

			return output("admin/council_control_result.html");
		}
	}
	else if (!strcmp(ByWhatString, "RESTRICT_COUNCILS_FROM_WAR"))
	{
		QUERY("COUNCIL_ID1", CouncilID1String);
		QUERY("COUNCIL_ID2", CouncilID2String);

		int
			CouncilID1 = as_atoi(CouncilID1String),
			CouncilID2 = as_atoi(CouncilID2String);

		CCouncil
			*Council1 = COUNCIL_TABLE->get_by_id(CouncilID1),
			*Council2 = COUNCIL_TABLE->get_by_id(CouncilID2);

		if (Council1 == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no council with ID %1$s."),
									dec2unit(CouncilID1)));
			return output("admin/council_control_error.html");
		}

		if (Council2 == NULL)
		{
			ITEM("ERROR_MESSAGE",
					(char *)format(GETTEXT("There is no council with ID %1$s."),
									dec2unit(CouncilID2)));
			return output("admin/council_control_error.html");
		}

		if (Council1->get_id() == Council2->get_id())
		{
			ITEM("ERROR_MESSAGE",
					GETTEXT("2 councils have the same ID."));
			return output("admin/council_control_error.html");
		}

		CDetachmentCouncilCouncil *
			DetachmentCouncilCouncil = DETACHMENT_COUNCIL_COUNCIL_TABLE->get_by_council_council(Council1, Council2);
		if (DetachmentCouncilCouncil == NULL)
		{
			CDetachmentCouncilCouncil *
				NewDetachmentCouncilCouncil = new CDetachmentCouncilCouncil();
			NewDetachmentCouncilCouncil->set_id(DETACHMENT_COUNCIL_COUNCIL_TABLE->get_new_id());
			NewDetachmentCouncilCouncil->set_type(CDetachmentCouncilCouncil::TYPE_WAR);
			NewDetachmentCouncilCouncil->set_council1(Council1);
			NewDetachmentCouncilCouncil->set_council2(Council2);

			DETACHMENT_COUNCIL_COUNCIL_TABLE->add_detachment(NewDetachmentCouncilCouncil);

			NewDetachmentCouncilCouncil->type(QUERY_INSERT);
			STORE_CENTER->store(*NewDetachmentCouncilCouncil);

			ITEM("RESULT_MESSAGE",
					GETTEXT("The councils have restriction from war now."));

			return output("admin/council_control_result.html");
		}
		else
		{
			DetachmentCouncilCouncil->type(QUERY_DELETE);
			STORE_CENTER->store(*DetachmentCouncilCouncil);

			DETACHMENT_COUNCIL_COUNCIL_TABLE->remove_detachment(DetachmentCouncilCouncil->get_id());

			ITEM("RESULT_MESSAGE",
					GETTEXT("The councils don't have restriction from war now."));

			return output("admin/council_control_result.html");
		}
	}
	else
	{
		ITEM("ERROR_MESSAGE", GETTEXT("ByWhatString seems wrong. Please try again."));

		return output("admin/council_control_error.html");
	}

//	system_log("end page handler %s", get_name());

	return output("admin/council_control_result.html");
}
