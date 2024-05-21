#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"

bool
CPageBlockade::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name() );

	if (aPlayer->has_siege_blockade_restriction() == true)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("Due to Imperial regulations, you may not siege for another %1$d seconds."), aPlayer->get_siege_blockade_restriction_time()));

		return output("war/war_error.html");
	}

	ITEM("STRING_TARGET", GETTEXT("Target"));

	static CString
		TargetListHTML;
	TargetListHTML.clear();

	TargetListHTML = aPlayer->war_target_list_html();

	if (TargetListHTML.length())
	{
		ITEM("TARGET_LIST", (char *)TargetListHTML);
	}
	else
	{
		TargetListHTML = "<TABLE WIDTH=\"550\" CELLSPACING=\"0\" CELLPADDING=\"0\" BORDERCOLOR=\"#2A2A2A\">\n";
		TargetListHTML += "<TR>\n";
		TargetListHTML += "<TD CLASS=tabletxt ALIGN=\"center\">\n";
		TargetListHTML += GETTEXT("You have no hostile relationships.");
		TargetListHTML += "</TD>\n";
		TargetListHTML += "</TR>\n";
		TargetListHTML += "</TABLE>\n";

		ITEM("TARGET_LIST", (char *)TargetListHTML);
	}

	ITEM("PLAYER_ID", aPlayer->get_game_id());

	ITEM("SELECT_PLAYER_MESSAGE",
			GETTEXT("If you cannot find a target from the list above,"
					" please type in the target player ID of the desired target"
					" in the box below."));

	ITEM("STRING_TARGET_PLAYER_ID", GETTEXT("Target Player ID"));

	CString
		InfoMenuURL;
	InfoMenuURL.clear();
	InfoMenuURL.format("<A HREF=\"/archspace/info/info.as\">%s</A>",
						GETTEXT("Info"));

	ITEM("PLAYER_SEARCH_INFORMATION",
			(char *)format(GETTEXT("You can search the player ID by name from %1$s in %2$s menu."),
							GETTEXT("Player Search"),
							(char *)InfoMenuURL));

//	system_log( "end page handler %s", get_name() );

	return output("war/blockade.html");
}

