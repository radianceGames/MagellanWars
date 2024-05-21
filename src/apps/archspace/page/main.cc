#include <libintl.h>
#include "../pages.h"
#include "../archspace.h"
#include "../game.h"

bool
CPageMain::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	CString BGColor;

	switch(aPlayer->get_race())
	{
		case 1:
			BGColor = "#2A1745";
			break;
		case 2:
			BGColor = "#0F3200";
			break;
		case 3:
			BGColor = "#462700";
			break;
		case 4:
			BGColor = "#003A48";
			break;
		case 5:
			BGColor = "#313131";
			break;
		case 6:
			BGColor = "#541E1E";
			break;
		case 7:
			BGColor = "#483600";
			break;
		case 8:
			BGColor = "#5A0000";
			break;
		case 9:
			BGColor = "#00342E";
			break;
		case 10:
			BGColor = "#3E3E00";
			break;
		case 11:
			BGColor = "#000000";
			break;
		default:
			BGColor = "";
	}

	ITEM("RACE_BG_COLOR", (char *)BGColor);

	CString
		ImageURL;
	ImageURL.clear();
	ImageURL.format("%s/image/as_game/race/", (char *)CGame::mImageServerURL);

	CString Race = aPlayer->get_race_name2();

	CString	Buf;
	Buf.format("%s%s/main_img.gif", (char *)ImageURL, (char *)Race);

	ITEM("IMAGE_MAIN", (char *)Buf);

	Buf = ImageURL + Race + "/main_symbol.jpg";
	ITEM("IMAGE_MAIN_SYMBOL", (char *)Buf);

	ITEM("TITLE",
			(char *)format("%s %s",
							aPlayer->get_honor_description(), aPlayer->get_nick() ) );

	ITEM("STRING_RANKING", GETTEXT("Ranking"));
	ITEM("STRING_PP", GETTEXT("PP"));
	ITEM("STRING_PLANET", GETTEXT("Planet"));
	ITEM("STRING_POPULATION", GETTEXT("Population"));
	ITEM("STRING_CONCENTRATION_MODE", GETTEXT("Concentration Mode"));
	ITEM("STRING_POWER", GETTEXT("Power"));
//	ITEM("STRING_VIRTUAL_POWER", GETTEXT("Virtual Power"));
	ITEM("STRING_FLEET", GETTEXT("Fleet"));
	ITEM("STRING_SHIP", GETTEXT("Ship"));
	ITEM("STRING_HONOR", GETTEXT("Honor"));
	ITEM("STRING_COUNCIL", GETTEXT("Council"));

	CRankTable *
		RankTable = PLAYER_TABLE->get_overall_rank_table();
	int
		Rank = RankTable->get_rank_by_id(aPlayer->get_game_id());

	if (Rank < 1)
	{
		ITEM("RANKING", GETTEXT("Not Available"));
	}
	else
	{
		ITEM("RANKING", dec2unit(Rank));
	}

	ITEM("PRODUCT_POINT", dec2unit(aPlayer->get_production()));
	ITEM("PLANET_COUNT", aPlayer->get_planet_list()->length());
	ITEM("POPULATION", (char *)format("%s k", dec2unit(aPlayer->calc_population())));
	ITEM("CONCENTRATION_MODE", aPlayer->get_mode_name());
	ITEM("POWER", dec2unit(aPlayer->get_power()));

	int
		Fleet = aPlayer->get_fleet_list()->length();
	ITEM("FLEET", dec2unit(Fleet));

	int
		Ship = aPlayer->get_fleet_list()->get_total_ship_number() + \
				aPlayer->get_dock()->get_ship_number();
	ITEM("SHIP", dec2unit(Ship));

	ITEM("HONOR", (char *)format("%s (%d)", aPlayer->get_honor_description(), aPlayer->get_honor()));

	CCouncil *
		Council = aPlayer->get_council();
	ITEM("COUNCIL", Council->get_nick());
/*
	CPurchasedProjectList *
		ProjectList = aPlayer->get_purchased_project_list();
	bool
		AnyEndingProject = false;
	CString
		EndingLink;

	for (int i=0 ; i<PERSONAL_ENDING_TABLE->length() ; i++)
	{
		CPersonalEnding *
			PersonalEnding = (CPersonalEnding *)PERSONAL_ENDING_TABLE->get(i);
		int
			ProjectID = PersonalEnding->get_project_id();
		CProject *
			EndingProject = (CProject *)ENDING_PROJECT_TABLE->get_by_id(ProjectID);
		if (EndingProject == NULL)
		{
			SLOG("ERROR : EndingProject is NULL in CPageMain::handler(), PersonalEnding->get_id() = %d, ProjectID = %d", PersonalEnding->get_id(), ProjectID);
			continue;
		}

		if (ProjectList->get_by_id(EndingProject->get_id()) != NULL)
		{
			EndingLink += "<TR>\n";
			EndingLink += "<TD ALIGN=center>\n";
			EndingLink += "<FORM ACTION=personal_ending.as METHOD=post>\n";
			EndingLink.format("<INPUT TYPE=hidden NAME=ENDING_ID VALUE=%d>",
								PersonalEnding->get_id());
			EndingLink.format("<INPUT TYPE=image SRC=\"%s/image/as_game/personal_ending%d>",
								(char *)CGame::mImageServerURL,
								PersonalEnding->get_id());
			EndingLink += "</FORM>\n";
			EndingLink += "</TD>\n";
			EndingLink += "</TR>\n";

			AnyEndingProject = true;
		}
	}
	if (AnyEndingProject == true)
	{
		ITEM("PERSONAL_ENDING_LINK", (char *)EndingLink);
	}
	else
	{*/
		ITEM("PERSONAL_ENDING_LINK", " ");
//	}

	if (CGame::mUpdateTurn) {
	char TimeString[60];
	struct tm *Time;
	time_t T = time(0);
	Time = localtime(&T);
	strftime(TimeString, 60, " %Y/%m/%d&nbsp;%r", Time);

	ITEM("SYSTEM_TIME", format(GETTEXT("Current : %1$s"), TimeString));

	T = aPlayer->get_tick()+CGame::mServerStartTime-CGame::mGameStartTime;
	Time = localtime(&T);
	strftime(TimeString, 60, " %Y/%m/%d&nbsp;%r", Time);
	ITEM("NEXT_TIME", format(GETTEXT("Next Turn : %1$s"), TimeString));
	//ITEM("CURRENT_DAY", format(GETTEXT("Day : %d"),GAME->get_game_time_in_days()));
	}
	else
	{
	ITEM("SYSTEM_TIME", GETTEXT("Time is not currently running."));
	ITEM("NEXT_TIME", " ");
	//ITEM("CURRENT_DAY", " ");
	}
	int UnreadDiplomaticMessages = aPlayer->get_diplomatic_message_box()->count_unread_message();

	if (UnreadDiplomaticMessages != 0)
	{

		ITEM("UNREAD_DIPLOMATIC_MESSAGE_NUMBER", format(GETTEXT("You have %1$d unread diplomatic message%2$s."),
					UnreadDiplomaticMessages, plural(UnreadDiplomaticMessages)));
	}
	else
	{
		ITEM("UNREAD_DIPLOMATIC_MESSAGE_NUMBER", " ");
	}


	CPlayer *
		Speaker = aPlayer->get_council()->get_speaker();

	int UnreadCouncilMessage = aPlayer->get_council()->get_council_message_box()->count_unread_message();


	if(UnreadCouncilMessage != 0)
	{
		if (Speaker != NULL)
		{
			if (Speaker->get_game_id() == aPlayer->get_game_id())
			{
				ITEM("UNREAD_COUNCIL_MESSAGE_NUMBER", format(GETTEXT("You have %1$d unread council message%2$s."),
							UnreadCouncilMessage, plural(UnreadCouncilMessage)));
			}
			else
			{
				ITEM("UNREAD_COUNCIL_MESSAGE_NUMBER", " ");
			}
		}
		else
		{
			ITEM("UNREAD_COUNCIL_MESSAGE_NUMBER", " ");
		}
	}
	else
	{
		ITEM("UNREAD_COUNCIL_MESSAGE_NUMBER", " ");
	}

	Buf = ImageURL + Race + "/main_system_advisory.gif";
	ITEM("IMAGE_SYSTEM_ADVISORY", (char *)Buf);

	static CString
		NewsString;
	NewsString.clear();

	NewsString = aPlayer->generate_news();

	if (strcmp((char *)NewsString, " "))
	{
		static CString
			SystemNews;
		SystemNews.clear();

		SystemNews = "<TR>\n";
		SystemNews += "<TD class=\"narrow\" align=\"center\" height=\"18\">";
		SystemNews += "<IMG SRC=" + ImageURL + Race + "/main_system_advisory.gif>";
		SystemNews += "</TD>\n";
		SystemNews += "</TR>\n";

		SystemNews += "<TR>\n";
		SystemNews += "<TD class=\"maintext\" ALIGN=\"CENTER\">\n";
		SystemNews += "<TABLE WIDTH=\"520\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";

		if(aPlayer->is_protected())
		{
			SystemNews += "You are Under the Protection of the Empire <BR>";
		}
		SystemNews += NewsString;

		SystemNews += "</TABLE>\n";
		SystemNews += "</TD>\n";
		SystemNews += "</TR>\n";

		SystemNews += "<TR>\n";
		SystemNews += "<TD class=\"maintext\" VALIGN=\"MIDDLE\" ALIGN=\"CENTER\">\n";
		SystemNews += "&nbsp;\n";
		SystemNews += "</TD>\n";
		SystemNews += "</TR>\n";

		ITEM("SYSTEM_ADVISORY", (char *)SystemNews);
	} else
	{
		ITEM("SYSTEM_ADVISORY", " ");
	}

/*
	if (There's any imperial happening)					// This should be modified.....
	{
		static CString
			ImperialNews;
		ImperialNews.clear();

		if (strcmp((char *)NewsString, " "))
		{
			ImperialNews = "<TR>\n";
			ImperialNews += "<TD class=\"maintext\" VALIGN=\"MIDDLE\" ALIGN=\"CENTER\">\n";
			ImperialNews += "<HR SIZE=\"1\" NOSHADE ALIGN=\"CENTER\" WIDTH=\"550\">\n";
			ImperialNews += "</TD>\n";
			ImperialNews += "</TR>\n";

			ImperialNews += "<TR>\n";
			ImperialNews += "<TD class=\"maintext\" VALIGN=\"MIDDLE\" ALIGN=\"CENTER\">\n";
			ImperialNews += "&nbsp;\n";
			ImperialNews += "</TD>\n";
			ImperialNews += "</TR>\n";
		}

		ImperialNews += "<TR>\n";
		ImperialNews += "<TD class=\"maintext\" ALIGN=\"CENTER\">";
		ImperialNews += "<IMG SRC=" + ImageURL + Race + "/main_imperial_happening.gif>";
		ImperialNews += "</TD>\n";
		ImperialNews += "</TR>\n";

		ImperialNews += "<TR>\n";
		ImperialNews += "<TD class=\"maintext\" ALIGN=\"CENTER\">\n";
		ImperialNews += "<TABLE WIDTH=\"520\" BORDER=\"0\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n";

		ImperialNews += "<TR>\n";

		ImperialNews += "<TD WIDTH=\"86\" CLASS=\"maintext\">\n";
		ImperialNews += "<FONT COLOR=\"#FFFFFF\" SIZE=\"2\">&nbsp;</FONT>\n";
		ImperialNews += "</TD>\n";

		ImperialNews += "<TD WIDTH=\"434\" CLASS=\"maintext\">\n";
		ImperialNews += "&nbsp;\n";
		ImperialNews += "</TD>\n";

		ImperialNews += "</TR>\n";

		ImperialNews += "</TABLE>\n";
		ImperialNews += "</TD>\n";
		ImperialNews += "</TR>\n";

		ITEM("IMPERIAL_HAPPENING", (char *)ImperialNews);
	}
*/
	ITEM("IMPERIAL_HAPPENING", " ");					// This should be deleted.....

	ITEM("SECOND_PER_TURN", CGame::mSecondPerTurn);

//	system_log("end page handler %s", get_name());

	return output("main.html");
}
