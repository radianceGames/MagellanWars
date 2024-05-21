#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"
#include "../../battle.h"

bool
CPageBattleReportSearch::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());
	CString
		buf,
		buf2;

	QUERY("TYPE", type);
	int
		queryType = as_atoi(type);
	int
		playerID,
		councilID;
	int
		attackerRace,
		defenderRace;

	// added by mind, lazy fix
	ITEM("BATTLE_LIST", "");
	ITEM("BATTLE_LIST_HEADER", "");
	if (queryType == 0)
	{
		QUERY("PLAYER", player);
		playerID = as_atoi(player);
	}
	else if (queryType == 1)
	{
		QUERY("COUNCIL", council);
		councilID = as_atoi(council);
	}
	else if (queryType == 2)
	{
		QUERY("ATTACKER", attacker);
		attackerRace = as_atoi(attacker);
		QUERY("DEFENDER", defender);
		defenderRace = as_atoi(defender);
	}

	int
		count = 0;
	for (int i=BATTLE_RECORD_TABLE->length()-1 ; i>=0 && count<50 ; i--)
	{
		CBattleRecord *
			record = (CBattleRecord *)BATTLE_RECORD_TABLE->get(i);
		if (queryType == 0)
		{	if(record->get_attacker_id() != playerID
				&& record->get_defender_id() != playerID )
				continue;
		}
		else if (queryType == 1)
		{	if(record->get_attacker_council() != councilID
				&& record->get_defender_council() != councilID )
				continue;
		}
		else if (queryType == 2)
		{	if(record->get_attacker_race() != attackerRace
				|| record->get_defender_race() != defenderRace )
				continue;
		}
		count++;
		buf.format("<TR>\n<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s (#%d)</TD>\n",
					record->get_attacker_name(), record->get_attacker_id());
		buf.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s (#%d)</TD>\n",
					record->get_defender_name(), record->get_defender_id());
		char TimeString[60];
		struct tm *Time;
		time_t T = record->get_time();
		Time = localtime(&T);
		strftime(TimeString, 60, " %Y/%m/%d&nbsp;%r", Time);
		buf.format("<TD CLASS=\"tabletxt\" ALIGN=\"LEFT\">%s</TD>\n", TimeString);
		buf.format("<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">%s</TD>\n",
					record->get_war_type_string() );
		
		if (record->get_winner() == record->get_attacker_id())
		{
			buf2 = GETTEXT("Attacker Win");
		}
		else if (record->get_winner() == record->get_defender_id())
		{
			buf2 = GETTEXT("Defender Win");
		}
		else
		{
			buf2 = GETTEXT("Draw");
		}
			
		buf = buf + "<TD CLASS=\"tabletxt\" ALIGN=\"CENTER\">" + buf2 + "</TD>\n";
		buf +=  "<TD ALIGN=\"CENTER\" VALIGN=\"CENTER\">";
		if (record->there_was_battle() == true)
		{
			buf.format("<FORM ACTION=\"battle_report2.as\" METHOD=POST><INPUT TYPE=HIDDEN NAME=LOG_ID VALUE=%d><INPUT TYPE=IMAGE SRC=\"%s/image/as_game/bu_read.gif\"></FORM>", record->get_id(), (char *)CGame::mImageServerURL);
		}
		else
		{
			buf += "No Battle";
		}
		buf += "</TD>\n";
		buf += "</TR>";
	}
	if (count)
	{
		ITEM("BATTLE_LIST_HEADER",
				"<TR BGCOLOR=\"#171717\">\n"
				"<TH CLASS=\"tabletxt\" WIDTH=\"136\">\n"
				"<FONT COLOR=\"666666\">Attacker </FONT>\n"
				"</TH>\n"
				"<TH CLASS=\"tabletxt\" WIDTH=\"136\">\n"
				"<FONT COLOR=\"666666\">Defender </FONT>\n"
				"</TH>\n"
				"<TH WIDTH=\"113\" CLASS=\"tabletxt\">\n"
				"<FONT COLOR=\"666666\">Date </FONT>\n"
				"</TH>\n"
				"<TH WIDTH=\"54\" CLASS=\"tabletxt\">\n"
				"<FONT COLOR=\"666666\">Type </FONT>\n"
				"</TH>\n"
				"<TH WIDTH=\"55\" CLASS=\"tabletxt\">\n"
				"<FONT COLOR=\"666666\">Result</FONT>\n"
				"</TH>\n"
				"<TH WIDTH=\"51\" CLASS=\"tabletxt\">&nbsp</TH>\n"
				"</TR>");
	}

	ITEM("BATTLE_LIST", buf);
	buf.clear();
	if (count == 1) 
		buf.format("1 report found");
	else 
		buf.format("%d reports were found", count);
	ITEM("SEARCH_RESULT", buf);

//	system_log("end page handler %s", get_name());

	return output( "war/battle_report_search.html" );
}

/* end of file battle_report_search.cc */
