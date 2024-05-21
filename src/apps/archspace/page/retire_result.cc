#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"

bool
CPageRetireResult::handle(CConnection &aConnection)
{
//	system_log("start page handler %s", get_name());

	if (!CPageCommon::handle(aConnection)) return false;
/*
	CString
		CurrentReferer,
		LegalReferer;
	CurrentReferer = aConnection.get_referer();
	LegalReferer.format("http://%s/archspace/retire.as", aConnection.get_host_name());

	if ((char *)CurrentReferer == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You seem to have approached this page in a wrong way."));
		return output("error.html");
	}*/
/*	 commented because using a non-standard port causes this to not work
if (strcmp((char *)CurrentReferer, (char *)LegalReferer))
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You seem to have approached this page in a wrong way."));
		return output("error.html");
	}*/

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
//#endif

	QUERY("ANSWER", AnswerString);
	if (AnswerString == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You have not decided to retire from Archspace or not."));
		return output("retire_error.html");
	}
	else if (!strcmp(AnswerString, "YES"))
	{
		Player->set_dead((char *)format("Player %s has retired. [%s]",
										Player->get_nick(),
										Player->get_last_login_ip()));
		Player->remove_from_database();
		Player->remove_from_memory();
		Player->remove_news_files();
		PLAYER_TABLE->remove_player(Player->get_game_id());

		ITEM("RESULT_MESSAGE", GETTEXT("You are dead now."));

		return output("retire_result.html");
	}
	else if (!strcmp(AnswerString, "NO"))
	{
		ITEM("RESULT_MESSAGE", GETTEXT("You have decided not to retire from Archspace."));
		return output("retire_result.html");
	}
	else
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You have not decided to retire from Archspace or not."));
		return output("retire_error.html");
	}

//	system_log("end page handler %s", get_name());
}
