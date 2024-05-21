#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"

bool
CPageQuit::handle(CConnection &aConnection)
{
//	system_log("start page handler %s", get_name());

	if (!CPageCommon::handle(aConnection)) return false;

	int PortalID = CONNECTION->get_portal_id();
	CPlayer 
		*Player = PLAYER_TABLE->get_by_portal_id(PortalID);
	CHECK(!Player, "<A HREF=/archspace/create.as>Move to create a character</A>");

	CString
		Buf;

	if (Player->is_dead() == false)
	{
		Buf += GETTEXT("You can quit only if your domain is demolished.");
		Buf += "<BR>\n";
		ITEM("ERROR_MESSAGE", (char *)Buf);
		return output("error.html");
	}

	SLOG("The player %s is dead.", Player->get_nick());

	CString
		PlayerNick;
	PlayerNick = Player->get_nick();

	Player->remove_from_database();
	Player->remove_from_memory();
	Player->remove_news_files();
	PLAYER_TABLE->remove_player(Player->get_game_id());

	ITEM("RESULT_MESSAGE", GETTEXT("You are dead."));

	ADMIN_TOOL->add_dead_player_log(
			(char *)format("Player %s has quited Archspace.", (char *)PlayerNick));

//	system_log("end page handler %s", get_name());

	return output("quit.html");
}
