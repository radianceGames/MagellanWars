#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageAdmissionRequest::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	int
		Speaker = aPlayer->get_council()->get_speaker_id();

	if (!aPlayer->can_request_admission())
	{
	 ITEM("ERROR_MESSAGE", aPlayer->admission_limit_time_left());
	 return output("council/admission_request_error.html");	 
	}
	if (aPlayer->get_game_id() == Speaker)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("The speaker cannot request admission to another council."));
		return output("council/admission_request_error.html");
	}

	if (aPlayer->get_admission())
	{
		ITEM("DELETE_ADMISSION_MESSAGE",
				GETTEXT("You already requested admission.<br> Do you want to delete it?"));
		return output("council/admission_request_delete.html");
	}

	if (aPlayer->get_council()->get_number_of_members() == 1)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You are the only member in your council."));
		return output("council/admission_request_error.html");
	}

	CCouncil *
		Council = aPlayer->get_council();

	if (!COUNCIL_TABLE->html_select_council_except(Council))
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("There's no council that you can send request."));
		return output("council/admission_request_error.html");
	}

	ITEM("SELECT_COUNCIL_MESSAGE",
			GETTEXT("Select the name of the council you want to join. "
					"Optionally, you can append an article."));

	ITEM("STRING_TO", GETTEXT("To"));

	ITEM("REQUEST_COUNCIL_SELECT",
			COUNCIL_TABLE->html_select_council_except(Council));

	ITEM("DEFAULT_HEADER_MESSAGE",
			GETTEXT("Message : I want to join your council. "
					"Please grant an admission to me."));

//	system_log("end page handler %s", get_name());

	return output("council/admission_request.html");
}
