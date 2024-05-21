#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageDeleteAdmissionRequestResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());
	CAdmission *admission = aPlayer->get_admission();
	
	if (admission) {
		CCouncil *council = admission->get_council();	
		
		admission->type(QUERY_DELETE);
		STORE_CENTER->store(*admission);
		aPlayer->reset_admission();
		if (council) {
			council->remove_admission(aPlayer);
		}

		CPlayer * CouncilSpeaker = council->get_speaker();
		if (CouncilSpeaker) {
			CouncilSpeaker->time_news((char *)format(GETTEXT("%1$s has removed admission into your council"), 
			aPlayer->get_nick()));
		}

		ITEM("RESULT_MESSAGE", 
			GETTEXT("The letter has been removed successfully."));
		
		return output("council/admission_request_result.html");
	} else {
		ITEM("ERROR_MESSAGE",
			GETTEXT("You have no admission request to delete"));
		return output("council/admission_request_error.html");
	}

//	system_log("end page handler %s", get_name());

	
}
