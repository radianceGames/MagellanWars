#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPagePlayerAdmissionAccuse::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	CCouncil *
		Council = aPlayer->get_council();

	QUERY("ADMISSION_TIME", AdmissionTimeString);
	time_t
		AdmissionTime = as_atoi(AdmissionTimeString);
	CAdmission *
		Admission = Council->get_admission_by_time(AdmissionTime);
	if (Admission == NULL)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("Your council doesn't have such an admission request."));
		return output("council/player_admission_accuse_error.html");
	}

	ADMIN_TOOL->accuse_message(Admission);

	ITEM("MENU_TITLE", GETTEXT("Player Admission Message - Accuse"));

	ITEM("RESULT_MESSAGE", GETTEXT("The message has been accused."));

//	system_log("end page handler %s", get_name());

	return output("council/player_admission_accuse.html");
}
