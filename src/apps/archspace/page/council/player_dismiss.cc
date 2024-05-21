#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"

bool
CPagePlayerDismiss::handler(CPlayer *aPlayer)
{

	ARE_YOU_SPEAKER(aPlayer);

	CCouncil *Council = aPlayer->get_council();

	ITEM("MENU_TITLE", GETTEXT("Player Dismissal"));
	
	static CString
		HTML;
	HTML.clear();
	
	char *Members = Council->get_dismissable_html();
	
	if (!Members) {
		HTML = "<TD CLASS=\"tabletxt\" ALIGN=\"center\" WIDTH=\"157\">No Players are Dismissable</TD>\n";
	} else {
		HTML = "<TD CLASS=\"tabletxt\" ALIGN=\"center\" WIDTH=\"157\">Select a Player: ";
		HTML += "<SELECT NAME=\"PLAYER_ID\">";
		HTML += Members;
		HTML += "</SELECT>";
		HTML.format(" <INPUT TYPE=image SRC=\"%s/image/as_game/bu_ok.gif\" BORDER=0>", (char *)CGame::mImageServerURL);
	}
	ITEM("MEMBERS", (char *)HTML);
	return output("council/player_dismiss.html");
}
