#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"

bool
CPageChangeCouncilSlogan::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	ITEM("MENU_TITLE", GETTEXT("Change Council Slogan"));

	CCouncil *
		Council = aPlayer->get_council();

	QUERY("COUNCIL_SLOGAN", OriginalCouncilSlogan);

	CString
		CouncilSlogan;
	CouncilSlogan = OriginalCouncilSlogan;
	CouncilSlogan.htmlspecialchars();
	CouncilSlogan.nl2br();

	if (!is_valid_slogan((char *)CouncilSlogan))
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Enter the council slogan."));
	}
	else
	{
		ITEM("RESULT_MESSAGE",
			(char *)format(GETTEXT("Council slogan has been changed to \"%1$s\"."),
					(char *)CouncilSlogan));
		Council->set_slogan((char *)CouncilSlogan);

		Council->type(QUERY_UPDATE);
		STORE_CENTER->store(*Council);
	}
//	system_log("end page handler %s", get_name());

	return output("council/speaker_menu_change_council_slogan_result.html");
}
