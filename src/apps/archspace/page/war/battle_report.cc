#include <libintl.h>
#include "../../pages.h"
#include "../../race.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageBattleReport::handler(CPlayer *aPlayer)
{
//	system_log( "start page handler %s", get_name());

#define BATTLE_RECORD_PER_PAGE		20

	QUERY("START_BATTLE_RECORD", StartBattleRecordString);
	int
		StartBattleRecord = as_atoi(StartBattleRecordString);
	StartBattleRecord = StartBattleRecord - (StartBattleRecord % BATTLE_RECORD_PER_PAGE) + 1;
	if (StartBattleRecord < 1) StartBattleRecord = 1;

	int
		TotalBattleRecord;
	ITEM("BATTLE_RECORD_LIST",
			aPlayer->battle_record_list_html(true, StartBattleRecord - 1, BATTLE_RECORD_PER_PAGE, &TotalBattleRecord));

	if (TotalBattleRecord > 0)
	{
		CString
			BattleRecordLink;

		int
			StartOrder = 1;
		while (1)
		{
			if (StartOrder != 1) BattleRecordLink += "&nbsp;&nbsp;";

			if (StartOrder + BATTLE_RECORD_PER_PAGE > TotalBattleRecord)
			{
				BattleRecordLink.format("<A HREF=\"battle_report.as?START_BATTLE_RECORD=%d\">%d - %d</A>",
										StartOrder,
										StartOrder,
										TotalBattleRecord);
				break;
			}
			else
			{
				BattleRecordLink.format("<A HREF=\"battle_report.as?START_BATTLE_RECORD=%d\">%d - %d</A>",
										StartOrder,
										StartOrder,
										StartOrder + BATTLE_RECORD_PER_PAGE - 1);

				StartOrder += BATTLE_RECORD_PER_PAGE;
			}
		}

		ITEM("BATTLE_RECORD_LINK", (char *)BattleRecordLink);
	}
	else
	{
		ITEM("BATTLE_RECORD_LINK", " ");
	}
#undef BATTLE_RECORD_PER_PAGE

	ITEM("SEARCH_BATTLE_MESSAGE",
			GETTEXT("Search the records of all battles the occurred in the Empire"));

	ITEM("STRING_QUERY_METHOD", GETTEXT("Query Method"));

	ITEM("STRING_PLAYER_ID", GETTEXT("Player ID"));
	ITEM("STRING_COUNCIL_ID", GETTEXT("Council ID"));
	ITEM("STRING_RACE", GETTEXT("Race"));

	static CString
		AttackerRaceSelect,
		DefenderRaceSelect;
	AttackerRaceSelect.clear();
	DefenderRaceSelect.clear();

	AttackerRaceSelect = "<SELECT NAME=\"ATTACKER\">\n";
	DefenderRaceSelect = "<SELECT NAME=\"DEFENDER\">\n";

	for (int i=CRace::RACE_BEGIN ; i<CRace::RACE_MAX-1 ; i++)
	{
		if (i == 0)
		{
			AttackerRaceSelect.format("<OPTION VALUE=\"%d\" SELECTED>%s</OPTION>\n",
								i, RACE_TABLE->get_name_by_id(i));
			DefenderRaceSelect.format("<OPTION VALUE=\"%d\" SELECTED>%s</OPTION>\n",
								i, RACE_TABLE->get_name_by_id(i));
		} else
		{
			AttackerRaceSelect.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
								i, RACE_TABLE->get_name_by_id(i));
			DefenderRaceSelect.format("<OPTION VALUE=\"%d\">%s</OPTION>\n",
								i, RACE_TABLE->get_name_by_id(i));
		}
	}

	AttackerRaceSelect += "</SELECT>\n";
	DefenderRaceSelect += "</SELECT>\n";

	ITEM("ATTACKER_RACE_SELECT", (char *)AttackerRaceSelect);
	ITEM("STRING_ATTACKING", GETTEXT("Attacking"));
	ITEM("DEFENDER_RACE_SELECT", (char *)DefenderRaceSelect);

	CString
		InfoMenuURL;
	InfoMenuURL.clear();
	InfoMenuURL.format("<A HREF=\"/archspace/info/info.as\">%s</A>",
						GETTEXT("Info"));

	ITEM("SEARCH_INFO_MESSAGE",
			(char *)format(GETTEXT("You can search the player ID and the council ID from %1$s menu and %2$s menu respectively in %3$s menu."),
							GETTEXT("Player Search"),
							GETTEXT("Council Search"),
							(char *)InfoMenuURL));

//	system_log("end page handler %s", get_name());

	return output( "war/battle_report.html" );
}

/* end of file battle_report.cc */
