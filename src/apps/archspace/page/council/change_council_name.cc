#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageChangeCouncilName::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	ITEM("MENU_TITLE", GETTEXT("Change Council Name"));

	CCouncil *Council = aPlayer->get_council();

	QUERY("COUNCIL_NAME", OriginalCouncilName);

	CString
		CouncilName;
	CouncilName = OriginalCouncilName;
	CouncilName.htmlspecialchars();
	CouncilName.nl2br();

	if (!is_valid_name((char *)CouncilName))
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Enter the council name."));
	} else if (!strcmp(Council->get_name(), (char *)CouncilName))
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("You should enter a different name from current name."));
	} else if (COUNCIL_TABLE->get_by_name((char *)CouncilName))
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("This name is used by another council."));
	} else
	{
		ITEM("RESULT_MESSAGE",
  				(char *)format(GETTEXT("Council name has been changed to \"%1$s\"."),
						(char *)CouncilName));
		Council->set_name((char *)CouncilName);

		Council->type(QUERY_UPDATE);
		STORE_CENTER->store(*Council);
	}

//	system_log("end page handler %s", get_name());

	return output("council/speaker_menu_change_council_name_result.html");
}
