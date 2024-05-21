#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../archspace.h"
#include "../../game.h"

bool
CPageCouncilProject::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);
	
	QUERY("PROJECT", ProjectString);
	int
		ProjectID = as_atoi(ProjectString);
	CProject *
		Project = PROJECT_TABLE->get_by_id(ProjectID);
	if (!Project)
	{
		ITEM("ERROR_MESSAGE",
				(char *)format(GETTEXT("There is no project with ID %1s"),
								dec2unit(ProjectID)));
		return output("council/speaker_menu_error.html");
	}
	ITEM("MENU_TITLE", GETTEXT("Project"));

	ITEM("RESULT_MESSAGE", 
			aPlayer->get_council()->buy_council_project(ProjectID));

//	system_log("end page handler %s", get_name());

	return output("council/speaker_menu_project_result.html");
}
