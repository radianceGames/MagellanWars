#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"

bool
CPageRetire::handle(CConnection &aConnection)
{
//	system_log("start page handler %s", get_name());

	if (!CPageCommon::handle(aConnection)) return false;

	int
		PortalID = CONNECTION->get_portal_id();
	CPlayer *
		Player = PLAYER_TABLE->get_by_portal_id(PortalID);
	if (Player == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You don't have any character. Click <A HREF=/archspace/create.as>here</A> to create a new character."));
		return output("error.html");
	}
//#ifdef RETIREMENT_COOLDOWN
	if ((Player->get_turn() * CGame::mSecondPerTurn < CPlayer::mRetireTimeLimit))
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Your character is too young to retire."));
		return output("error.html");
	}
	//else if ((!GAME->mUpdateTurn) && )
	//{
	//
	//}
//#endif

	ITEM("CONFIRM_MESSAGE",
			GETTEXT("Are you sure that you will retire?"));

//	system_log("end page handler %s", get_name());

	return output("retire.html");
}
