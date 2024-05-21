#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"

bool
CPageDeathMain::handle(CConnection &aConnection)
{
//	system_log("start page handler %s", get_name());
	if (!CPageCommon::handle(aConnection)) return false;

	int
		PortalID = CONNECTION->get_portal_id();
	CPlayer *
		Player = PLAYER_TABLE->get_by_portal_id(PortalID);
	if (!Player)
	{
		message_page(GETTEXT("Your character was not found."));

		return true;
	}
	if (!Player->is_dead())
	{
		message_page(GETTEXT("Your character is alive."));

		return true;
	}

	CString
		QuitURL,
		RestartURL;
	QuitURL.clear();
	RestartURL.clear();

	QuitURL = "/archspace/quit.as";
	RestartURL = "/archspace/restart.as";

	ITEM("DEATH_MESSAGE",
			(char *)format(GETTEXT("You are dead. You may <A HREF=\"%s\">quit</A> or <A HREF=\"%s\">restart</A>."),
							(char *)QuitURL,
							(char *)RestartURL));

//	system_log("end page handler %s", get_name());

	return output("death_main.html");
}
