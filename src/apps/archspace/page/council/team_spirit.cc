#include <libintl.h>
#include "../../pages.h"

bool
CPageTeamSpirit::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ITEM("MENU_TITLE", GETTEXT("Team Spirit"));

	ITEM("RESULT_MESSAGE",
			GETTEXT("Team spirit has been finished successfully.<BR>"
					"The fleet in your council is more powerful."));

//	system_log("end page handler %s", get_name());

	return output("council/speaker_menu_team_spirit_result.html");
}
