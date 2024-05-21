#include <libintl.h>
#include "../../pages.h"

bool
CPageSpy::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	static CString
		SpyList;
	SpyList.clear();

	SpyList = aPlayer->available_spy_list_html();
	//if (SpyList.length() == 0)
	//{
	//	ITEM("ERROR_MESSAGE",
	//			GETTEXT("You can't perform any spy mission"
	//					" because your PP is not enough"
	//					" or you don't have any prerequisite tech."));
	//	return output("diplomacy/spy_error.html");
	//}

	ITEM("STRING_SECURITY_LEVEL", GETTEXT("Security Level"));
	int
		SecurityLevel = aPlayer->get_security_level();
	ITEM("SECURITY_LEVEL_DESCRIPTION", CSpy::get_security_level_description(SecurityLevel));

	ITEM("STRING_ALERTNESS", GETTEXT("Alertness"));
	ITEM("ALERTNESS", dec2unit(aPlayer->get_alertness()));

	ITEM("STRING_TARGET_PLAYER", GETTEXT("Target Player"));

	ITEM("STRING_YOUR_COUNCIL_MEMBER_S_", GETTEXT("Your Council Member(s)"));
	CCouncil *
		Council = aPlayer->get_council();
	if (Council->get_number_of_members() == 1)
	{
		ITEM("TARGET_PLAYER", GETTEXT("There is only you in your council."));
	}
	else
	{
		ITEM("TARGET_PLAYER", Council->select_member_except_player_html(aPlayer));
	}

	ITEM("STRING_OR_INPUT_PLAYER_ID",
			GETTEXT("Or input the player's ID"));

	ITEM("SELECT_SPY_MESSAGE", GETTEXT("Select the Special Ops. from below :"));

	ITEM("AVAILABLE_SPY_LIST", (char *)SpyList);

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/spy.html" );
}
