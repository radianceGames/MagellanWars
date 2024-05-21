#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>

bool
CPagePlayerAdmissionRead::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	CCouncil *Council = aPlayer->get_council();

	QUERY("NO", ReadNumber);
	if (!ReadNumber)	
	{
		ITEM("RESULT_MESSAGE", 
				GETTEXT("Could not find the admission request record."));
		return output("council/speaker_menu_error.html");
	}

	int Number = as_atoi(ReadNumber);
	if (!Number)	
	{
		ITEM("RESULT_MESSAGE", 
				GETTEXT("Could not find the admission request record."));
		return output("council/speaker_menu_error.html");
	}

	CAdmission *Admission = Council->get_admission_by_index(Number-1);
	if (!Admission)
	{
		ITEM("RESULT_MESSAGE", 
				GETTEXT("Could not find the admission request record."));
		return output("council/speaker_menu_error.html");
	}

	ITEM("MENU_TITLE", GETTEXT("Player Admission"));

	ITEM("STRING_NO_", GETTEXT("No."));
	ITEM("STRING_SENDER", GETTEXT("Sender"));
	ITEM("STRING_DATE", GETTEXT("Date"));

	//ITEM("NO_", Number+1);
	ITEM("NO_", Number);
	ITEM("FROM", Admission->get_player()->get_nick());
	ITEM("DATE", Admission->get_time_string());


	// start telecard 2001/01/22
	ITEM("ADMISSION", Number );
	ITEM("PLAYER", Admission->get_player()->get_game_id() );
	// end telecard

	CString Message = Admission->get_content();
	ITEM("HEADER_MESSAGE", (char *)nl2br((char *)htmlspecialchars((char *)Message)));

	ITEM("STRING_YOUR_REPLY", GETTEXT("Your Reply"));
	ITEM("STRING_YES", GETTEXT("Yes"));
	ITEM("STRING_NO", GETTEXT("No"));
	ITEM("STRING_WELL", GETTEXT("Well"));

	ITEM("STRING_COMMENT", GETTEXT("Comment"));

	CString
		Accuse;
	Accuse.clear();

	Accuse.format("<A HREF=\"player_admission_accuse.as?"
												"ADMISSION_TIME=%d\">",
					Admission->get_time());
	Accuse += GETTEXT("Accuse the sender of improper language.");
	Accuse += "</A>";

	ITEM("ACCUSE_MESSAGE", (char *)Accuse);

//	system_log("end page handler %s", get_name());

	return output("council/player_admission_read.html");
}
