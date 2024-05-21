#include <libintl.h>
#include "../../pages.h"

bool
CPagePlayerAdmission::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ARE_YOU_SPEAKER(aPlayer);

	CCouncil *Council = aPlayer->get_council();

	ITEM("MENU_TITLE", GETTEXT("Player Admission"));

	const char *Table = Council->admission_table_html();

	CString AdmissionHTML;

	if (!Table)
	{
		AdmissionHTML = "<TR>\n"
					"<TD ALIGN=CENTER CLASS=\"maintext\">\n";
		AdmissionHTML += GETTEXT("No admission requests.");
		AdmissionHTML += "</TD>\n"
					"</TR>\n"
					"<TR>\n"
					"<TD>&nbsp;</TD>\n"
					"</TR>\n";

		ITEM("PLAYER_ADMISSION_TABLE", AdmissionHTML);
	} else {
		AdmissionHTML = "<TR>\n"
						"<TD ALIGN=CENTER CLASS=\"maintext\">\n";
		AdmissionHTML += GETTEXT("New admission request message(s) arrived.");
		AdmissionHTML += "</TD>\n"
						"</TR>\n"
						"<TR>\n"
						"<TD>&nbsp;</TD>\n"
						"</TR>\n"
						"<TR>\n"
						"<TD ALIGN=CENTER>\n";

		AdmissionHTML += "<TABLE WIDTH=450 BORDER=1 CELLSPACING=0 CELLPADDING=0 "
							"BORDERCOLOR=\"#2A2A2A\">\n"
						"<TR>\n"
						"<TH CLASS=\"tabletxt\" WIDTH=36 BGCOLOR=\"#171717\">&nbsp;";
		AdmissionHTML += GETTEXT("No.");
		AdmissionHTML += "</TH>\n"
						"<TH CLASS=\"tabletxt\" WIDTH=216 BGCOLOR=\"#171717\">";
		AdmissionHTML += GETTEXT("Name");
		AdmissionHTML += "</TH>\n"
						"<TH CLASS=\"tabletxt\" WIDTH=180 BGCOLOR=\"#171717\">";
		AdmissionHTML += GETTEXT("Date");
		AdmissionHTML += "</TH>\n"
						"<TH CLASS=\"tabletxt\" WIDTH=64 BGCOLOR=\"#171717\">";
		AdmissionHTML += GETTEXT("Read");
		AdmissionHTML += "</TH>\n"
						"</TR>\n";
		
		AdmissionHTML += Table;

		AdmissionHTML += "</TABLE>\n"
						"</TD>\n"
						"</TR>\n";
	}

	ITEM("PLAYER_ADMISSION_TABLE", AdmissionHTML);

	ITEM("STRING_NEW_PLAYER_OPTION", GETTEXT("New Player Assignment Option"));

	ITEM("STRING_ON", GETTEXT("On"));
	ITEM("STRING_OFF", GETTEXT("Off"));

	ITEM("SELECT_ON", (Council->get_auto_assign()) ? "CHECKED":" ");
	ITEM("SELECT_OFF", (!Council->get_auto_assign()) ? "CHECKED":" ");

//	system_log("end page handler %s", get_name());

	return output("council/player_admission.html");
}
