#include <libintl.h>
#include "../../pages.h"
#include "../../game.h"

void 
CPageDiplomacy::message_page(const char *aMessage)
{
	ITEM("RESULT_MESSAGE", aMessage);
	output("diplomacy/diplomacy_message.html");
}

bool
CPageDiplomacy::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	ITEM("DIPLOMACY_STATUS_LIST_EXPLAIN",
		GETTEXT("Shows the relations between you and the other members in your council."));
	ITEM("READ_MESSAGE_EXPLAIN",
		GETTEXT("You can read diplomatic messages from other councils."));
	ITEM("SEND_MESSAGE_EXPLAIN",
		GETTEXT("You can send various diplomatic messages to other councils."));
	ITEM("INSPECT_OTHER_PLAYER_EXPLAIN",
		GETTEXT("You can inspect the status of other players."));
	ITEM("SPECIAL_OPERATION_EXPLAIN", GETTEXT("Perform spy action or special operation."));

	const char* MessageTable 
			= aPlayer->diplomatic_unread_message_table_html();

	if (!MessageTable)
	{
		ITEM("FOOT_NOTE", " ");
		ITEM("DIPLOMATIC_MESSAGE_TABLE",
				GETTEXT("There are no unread diplomatic messages."));

		return output( "diplomacy/diplomacy.html" );
	}

	CString FootNote;

	FootNote =
		"<TABLE WIDTH=\"529\" BORDER=\"0\" CELLSPACING=\"0\""
				" CELLPADDING=\"0\">\n"
        "<TR VALIGN=TOP>\n"
		"<TD CLASS=\"maintext\" HEIGHT=24 WIDTH=244>&nbsp;</TD>\n"
		"<TD CLASS=\"maintext\" HEIGHT=24 WIDTH=28 ALIGN=RIGHT>&nbsp;</TD>\n"
		"<TD CLASS=\"maintext\" HEIGHT=24 WIDTH=257 ALIGN=RIGHT>";
	FootNote += GETTEXT("Reply Required");
	FootNote.format("<IMG SRC=\"%s/image/as_game/council/required1_icon.gif\" WIDTH=19 HEIGHT=16 ALIGN=ABSBOTTOM>\n&nbsp;&nbsp;&nbsp;&nbsp;",
					(char *)CGame::mImageServerURL);
	FootNote += GETTEXT("Not Required");
	FootNote.format("<IMG SRC=\"%s/image/as_game/council/notrequired1_icon.gif\" WIDTH=19 HEIGHT=16 ALIGN=ABSBOTTOM>\n</TD>\n</TR>\n</TABLE>\n",
					(char *)CGame::mImageServerURL);

	ITEM("FOOT_NOTE", FootNote);

	CString Table;
    Table =
		"<TABLE WIDTH=550 BORDER=1 CELLSPACING=0 CELLPADDING=0 BORDERCOLOR=\"#2A2A2A\">\n"
		"<TR>\n"
			"<TH CLASS=\"tabletxt\" WIDTH=32 BGCOLOR=\"#171717\">\n"
			"<FONT COLOR=\"666666\">&nbsp;";
	Table += GETTEXT("ID");
	Table += "</FONT></TH>\n"
			"<TH WIDTH=149 CLASS=\"tabletxt\" BGCOLOR=\"#171717\">"
			"<FONT COLOR=\"666666\">";
	Table += GETTEXT("Name");
	Table += "</FONT></TH>\n"
			"<TH WIDTH=119 CLASS=\"tabletxt\" BGCOLOR=\"#171717\">"
			"<FONT COLOR=\"666666\">";
	Table += GETTEXT("Date");
	Table += "</FONT></TH>\n"
			"<TH CLASS=\"tabletxt\" WIDTH=119 BGCOLOR=\"#171717\">"
			"<FONT COLOR=\"666666\">";
	Table += GETTEXT("Type");
	Table += "</FONT></TH>\n"
			"<TH CLASS=\"tabletxt\" WIDTH=47 BGCOLOR=\"#171717\">"
			"<FONT COLOR=\"666666\">";
	Table += GETTEXT("Status");
	Table += "</FONT></TH>\n";
	Table += "<TH CLASS=\"tabletxt\" WIDTH=70 BGCOLOR=\"#171717\">"
			"<FONT COLOR=\"666666\">";
	Table += GETTEXT("Read");
	Table += "</FONT></TH>\n"
			"</TR>\n";

	Table += MessageTable;

	Table += "</TABLE>\n";

	ITEM("DIPLOMATIC_MESSAGE_TABLE", Table);

//	system_log("end page handler %s", get_name());

	return output( "diplomacy/diplomacy.html" );
}
