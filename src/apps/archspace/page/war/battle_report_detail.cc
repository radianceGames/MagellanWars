#include <libintl.h>
#include "../../pages.h"
#include <cstdlib>
#include "../../battle.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageBattleReportDetail::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

	CString
		buf,
		buf2;
	QUERY( "LOG_ID", logIDstring );
	if( logIDstring ) {
		int logID = as_atoi( logIDstring ) ;
		CBattleRecord
			*record = (CBattleRecord*)BATTLE_RECORD_TABLE->get_by_id(logID);
		if( record != NULL ) {
			//ITEM( "LOG_ID", logIDstring );

			buf.format( "%s (#%d)", record->get_attacker_name(), record->get_attacker_id() );
			ITEM( "ATTACKER", buf );
			buf = "";
			buf.format( "%s (#%d)", record->get_defender_name(), record->get_defender_id() );
			ITEM( "DEFENDER", buf );
			char TimeString[60];
			struct tm *Time;
			time_t T = record->get_time();
			Time = localtime(&T);
			strftime(TimeString, 60, " %Y/%m/%d&nbsp;%r", Time);
			ITEM( "DATE", TimeString );
			buf = "";
			buf.format( "%s", record->get_war_type_string() );
			ITEM( "TYPE", buf );
			buf = "";
			buf.format( "%s", record->get_battle_field_name() );
			ITEM( "PLANET", buf );
			buf = "";
			if( record->get_winner() == 0)
            {
	             buf = GETTEXT("Draw");
			}
			else if( record->get_winner() == record->get_attacker_id() )
			{
				buf = GETTEXT("Attacker Win");
			}
			else if( record->get_winner() == record->get_defender_id() )
			{
				buf = GETTEXT("Defender Win");
			}
			else
			{
				buf = GETTEXT("Unknown");
			}
			
			ITEM( "RESULT", buf );
			if( record->get_attacker_id() == aPlayer->get_game_id() )
			{
				// gain
				if(record->get_attacker_gain() != NULL)
				{
					buf.format("<TR>\n<TD CLASS=\"tabletxt\" WIDTH=\"149\" BGCOLOR=\"#171717\">&nbsp;Gain</TD>\n<TD COLSPAN=\"3\" CLASS=\"tabletxt\" WIDTH=\"395\">&nbsp;</TD>\n</TR>\n<TR>\n<TD CLASS=\"tabletxt\" COLSPAN=\"4\" ALIGN=\"CENTER\">\n<TABLE BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\" WIDTH=\"450\">\n<TR>\n<TD CLASS=\"maintext\" WIDTH=\"108\" VALIGN=\"top\">&nbsp;Planet</TD><TD CLASS=\"maintext\" VALIGN=\"top\" COLSPAN=\"4\">&nbsp;%s</TD>\n</TR>\n</TABLE>\n<P>&nbsp;</P>\n</TD>\n</TR>\n", record->get_attacker_gain() );
					ITEM( "GAIN", buf );
				}
				else
				{
					ITEM( "GAIN", " " );
				}
				// lose
				if(record->get_attacker_lose_fleet() != NULL)
				{
					buf = "<TR>\n<TD CLASS=\"tabletxt\" WIDTH=\"149\" BGCOLOR=\"#171717\" VALIGN=\"TOP\">&nbsp;Lose</TD>\n<TD CLASS=\"tabletxt\" COLSPAN=\"3\" WIDTH=\"395\">&nbsp;</TD>\n</TR>\n<TR>\n<TD CLASS=\"tabletxt\" COLSPAN=\"4\" ALIGN=\"CENTER\">\n<TABLE BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\" WIDTH=\"450\">\n";
					buf.format( "<TR>\n<TD CLASS=\"maintext\" WIDTH=\"108\" VALIGN=\"top\">Fleet</TD>\n<TD CLASS=\"maintext\" VALIGN=\"top\" COLSPAN=\"4\">%s</TD>\n</TR>\n", (char*)record->get_attacker_lose_fleet() );
					buf.format( "<TR>\n<TD CLASS=\"maintext\" WIDTH=\"108\" VALIGN=\"top\">Commander</TD>\n<TD CLASS=\"maintext\" VALIGN=\"top\" COLSPAN=\"4\">%s</TD>\n</TR>\n", (char*)record->get_attacker_lose_admiral() );
					buf += "</TABLE>\n<BR>\n</TD>\n</TR>\n";
					ITEM( "LOSE", buf );
				}
				else
				{
					ITEM( "LOSE", " " );
				}
			}
			else if( record->get_defender_id() == aPlayer->get_game_id() )
			{
				ITEM( "GAIN", " " );
				// lose
				//ITEM( "LOSE", record->get_defender_lose() );
				if( record->get_defender_lose_fleet() != NULL)
				{
					buf.clear();
					buf = "<TR>\n<TD CLASS=\"tabletxt\" WIDTH=\"149\" BGCOLOR=\"#171717\" VALIGN=\"TOP\">&nbsp;Lose</TD>\n<TD CLASS=\"tabletxt\" COLSPAN=\"3\" WIDTH=\"395\">&nbsp;</TD>\n</TR>\n<TR>\n";
					buf += "<TD CLASS=\"tabletxt\" COLSPAN=\"4\" ALIGN=\"CENTER\">\n<TABLE BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\" WIDTH=\"450\">\n";
					buf.format( "<TR>\n<TD CLASS=\"maintext\" WIDTH=\"108\" VALIGN=\"top\">Fleet</TD>\n<TD CLASS=\"maintext\" VALIGN=\"top\" COLSPAN=\"4\">%s</TD></TR>\n", record->get_defender_lose_fleet() );
					buf.format( "<TR>\n<TD CLASS=\"maintext\" WIDTH=\"108\" VALIGN=\"top\">Commander</TD>\n<TD CLASS=\"maintext\" VALIGN=\"top\" COLSPAN=\"4\">%s</TD>\n</TR>\n", record->get_defender_lose_admiral() );
					buf += "</TABLE>\n<BR>\n</TD>\n</TR>\n";
					ITEM( "LOSE", buf );
				}
			}
			if( record->there_was_battle() == true )
			{
				buf = "<FORM ACTION=\"battle_report2.as\" METHOD=POST>";
				buf.format( "<INPUT TYPE=HIDDEN NAME=LOG_ID VALUE=%d>", logID );
				buf.format("<INPUT TYPE=IMAGE SRC=\"%s/image/as_game/bu_view.gif\"></FORM>",
							(char *)CGame::mImageServerURL);
				ITEM("VIEW", buf);
			} else
			{
				ITEM("VIEW", GETTEXT("No Fleet Battle") );
			}
		} else {
			ITEM( "ERROR_MESSAGE", "Battle report you requested not exist" );
			return output( "error.html" );
		}

	}

//	system_log("end page handler %s", get_name());

	return output( "war/battle_report_detail.html" );
}

/* end of file battle_report_detail.cc */
