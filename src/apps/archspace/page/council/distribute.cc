#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>

bool
CPageDistribute::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	ITEM("MENU_TITLE", GETTEXT("Distribute"));

	CCouncil *Council = aPlayer->get_council();

	QUERY("DISTRIBUTE", Distribute);
	if (!Distribute)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("Enter amount of PP for distribution."));
		return output("council/speaker_menu_distribute_result.html");
	}

	int Dist = as_atoi(Distribute);

	if (Dist < 1)
	{
		ITEM("RESULT_MESSAGE", GETTEXT("You must enter number more than 0."));
		return output("council/speaker_menu_distribute_result.html");
	}

	if (Dist > Council->get_production())
	{
		ITEM("RESULT_MESSAGE", GETTEXT("You enterd more PP than your council has."));
		return output("council/speaker_menu_distribute_result.html");
	}

	CString List;
	Dist = Council->distribute(Dist, List);

	if (Dist)
	{
		ITEM("RESULT_MESSAGE",
			(char *)format(
					GETTEXT("You distributed %1$s PP successfully."),
					dec2unit(Dist)));
		ITEM("DISTRIBUTE_MEMBER_LIST", List);
	}
	else
	{
		ITEM("RESULT_MESSAGE",
				GETTEXT("You couldn't distribute anything."));
		ITEM("DISTRIBUTE_MEMBER_LIST", " ");
	}

//	system_log("end page handler %s", get_name());

	return output("council/speaker_menu_distribute_result.html");
}
