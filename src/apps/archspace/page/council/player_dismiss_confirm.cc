#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"

bool
CPagePlayerDismissConfirm::handler(CPlayer *aPlayer)
{

	ARE_YOU_SPEAKER(aPlayer);

	CCouncil *Council = aPlayer->get_council();

	ITEM("MENU_TITLE", GETTEXT("Player Dismissal"));
	
	QUERY("PLAYER_ID", PlayerIDString);
	if (!PlayerIDString) {
		ITEM("CONFIRM", GETTEXT("<TD CLASS=\"tabletxt\" ALIGN=\"center\" WIDTH=\"157\">Invalid Player ID</TD>"));
		return output("council/player_dismiss_confirm.html");
	}
	int PlayerID = as_atoi(PlayerIDString);
	
	CPlayer *
		PlayerTarget = Council->get_member_by_game_id(PlayerID);
	if (!PlayerTarget) {
		ITEM("CONFIRM", GETTEXT("<TD CLASS=\"tabletxt\" ALIGN=\"center\" WIDTH=\"157\">Player does not belong to your council</TD>"));
		return output("council/player_dismiss_confirm.html");
	}
	
	if (!PlayerTarget->is_dismissable()) {
		ITEM("CONFIRM", GETTEXT("<TD CLASS=\"tabletxt\" ALIGN=\"center\" WIDTH=\"157\">Player is not dismissable</TD>"));
		return output("council/player_dismiss_confirm.html");
	}
	
	static CString
		HTML;
	HTML.clear();
	
	HTML.format("<TD CLASS=\"tabletxt\" ALIGN=\"center\" WIDTH=\"157\">Are you sure you want to dismiss %s?",PlayerTarget->get_nick());
	HTML.format(" <INPUT TYPE=hidden Name=PLAYER_ID Value=\"%d\"><INPUT TYPE=image SRC=\"%s/image/as_game/bu_ok.gif\" BORDER=0></TD>", PlayerID, (char *)CGame::mImageServerURL);
	
	ITEM("CONFIRM", (char *)HTML);
	return output("council/player_dismiss_confirm.html");
}
