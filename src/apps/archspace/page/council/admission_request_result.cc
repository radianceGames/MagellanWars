#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageAdmissionRequestResult::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	CCouncil *
		Council = aPlayer->get_council();
	CHECK(!aPlayer->can_request_admission(), aPlayer->admission_limit_time_left());
 	CHECK(Council->get_speaker_id() == aPlayer->get_game_id(),
				GETTEXT("A speaker can't make admission request to other council."));

	CHECK(aPlayer->get_admission(), 
			GETTEXT("You already requested admission."));

	QUERY("COUNCIL_SELECT", TargetCouncilIDString);
	QUERY("ADMISSION_MESSAGE", AdmissionMessage);

	int
		TargetCouncilID = as_atoi(TargetCouncilIDString);
	CCouncil *
		TargetCouncil = COUNCIL_TABLE->get_by_id(TargetCouncilID);
	CHECK(TargetCouncil == NULL, GETTEXT("There is no such a council."));

	CHECK(TargetCouncil->get_speaker() == NULL,
			GETTEXT("That council has has no speaker."));

	if (ADMIN_TOOL->are_they_restricted(aPlayer, TargetCouncil) == true)
	{
		message_page(GETTEXT("You can't send that kind of message because of the punishment."));
		return true;
	}

	CPlayerList *
		TargetCouncilMemberList = TargetCouncil->get_members();
	for (int i=0 ; i<TargetCouncilMemberList->length() ; i++)
	{
		CPlayer *
			TargetCouncilMember = (CPlayer *)TargetCouncilMemberList->get(i);
		if (ADMIN_TOOL->are_they_restricted(aPlayer, TargetCouncilMember) == true)
		{
			message_page(
					(char *)format(GETTEXT("You can't be in the same council with %1$s because of the punishment."),
									TargetCouncilMember->get_nick()));
			return true;
		}
	}

	if (Council->get_number_of_members() == 1)
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("You are the only member in your council."));
		return output("council/admission_request_error.html");
	}

	if (TargetCouncil->get_number_of_members() >= TargetCouncil->max_member())
	{
		ITEM("ERROR_MESSAGE",
				GETTEXT("We are sorry, this council is full and is no longer accepting applicants."));
		return output("council/admission_request_error.html");
	}

	CAdmission *
		Admission = new CAdmission();
	Admission->set_player(aPlayer);
	Admission->set_council(TargetCouncil);
	Admission->set_time(time(0));
	Admission->set_content(AdmissionMessage);

	CPlayer *
		TargetCouncilSpeaker = TargetCouncil->get_speaker();

	TargetCouncilSpeaker->time_news((char *)format(GETTEXT("%1$s has requested admission into your council"), 
			aPlayer->get_nick()));

	aPlayer->set_admission(Admission);
	TargetCouncil->add_admission(Admission);

	Admission->type(QUERY_INSERT);
	STORE_CENTER->store(*Admission);
	
	ITEM("RESULT_MESSAGE", 
		GETTEXT("The letter has been sent successfully."));

//	system_log("end page handler %s", get_name());

	return output("council/admission_request_result.html");
}
