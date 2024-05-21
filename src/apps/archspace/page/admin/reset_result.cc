#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../race.h"

bool
CPageResetResult::handler(CPlayer *aPlayer)
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


	QUERY("RESET", ResetString);
	if (ResetString == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You seem to have approached this page in a wrong way."));
		return output("admin/admin_error.html");
	}
	if (strcmp((char *)ResetString, "RESET"))
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You seem to have approached this page in a wrong way."));
		return output("admin/admin_error.html");
	}

	QUERY("CONQUEROR_NAME", ConquerorNameString);
	if (ConquerorNameString == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't specify the conqueror's name."));
		return output("admin/admin_error.html");
	}

	QUERY("CONQUEROR_COUNCIL_NAME", ConquerorCouncilNameString);
	if (ConquerorCouncilNameString == NULL)
	{
		ITEM("ERROR_MESSAGE", GETTEXT("You didn't specify a name of the conqueror's council."));
		return output("admin/admin_error.html");
	}

	CCouncil *
		ConquerorCouncil = new CCouncil();
	ConquerorCouncil->set_name((char *)ConquerorCouncilNameString);
	COUNCIL_TABLE->add_council(ConquerorCouncil);

	CPlayer *
		Conqueror = new CPlayer(PLAYER_TABLE->get_max_id() + 1);
	CRace *
		RandomRace = (CRace *)RACE_TABLE->get(number(RACE_TABLE->length()) - 1);
	Conqueror->set_name((char *)ConquerorNameString);
	Conqueror->set_race(RandomRace->get_id());
	Conqueror->set_council(ConquerorCouncil);

	PLAYER_TABLE->add_player(Conqueror);

	Conqueror->type(QUERY_INSERT);
	STORE_CENTER->store(*Conqueror);

	ConquerorCouncil->type(QUERY_INSERT);
	STORE_CENTER->store(*ConquerorCouncil);

	EMPIRE_CAPITAL_PLANET->set_owner_id(Conqueror->get_game_id());

	EMPIRE_CAPITAL_PLANET->type(QUERY_UPDATE);
	STORE_CENTER->store(*EMPIRE_CAPITAL_PLANET);
	ADMIN_TOOL->add_admin_action_log(thisAdmin, "Reset the game");
	ITEM("RESULT_MESSAGE",
			(char *)format(GETTEXT("The Empire has been conquered by %1$s. Now the game is over."),
							Conqueror->get_nick()));
    // Generic Strings
	ITEM("ADMIN_LINK_STRING", GETTEXT("[Admin Main]"));
 	ITEM("INSPECT_LINK_STRING", GETTEXT("[Inspection]"));
 	ITEM("BLACK_MARKET_LINK_STRING", GETTEXT("[Black Market Control]"));
 	ITEM("CONFIG_LINK_STRING", GETTEXT("[Modify Config]"));
   	ITEM("RESTRICTION_LINK_STRING", GETTEXT("[Restriction Control]"));

//	system_log("end page handler %s", get_name());

	return output("admin/reset_result.html");
}
