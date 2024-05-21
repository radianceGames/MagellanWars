#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageWhitebook::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	CString
		ImageURL;
	ImageURL.clear();
	ImageURL.format("%s/image/as_game/race/", (char *)CGame::mImageServerURL);
	CString
		Race = aPlayer->get_race_name2();

	static CString
		Image;
	Image.clear();
	Image = ImageURL + Race + "/symbol.gif";

	ITEM("IMAGE_RACE_SYMBOL", (char *)Image);

	ITEM("STRING_CHARACTER_NAME", GETTEXT("Character Name"));
	ITEM("STRING_RANKING", GETTEXT("Ranking"));
	ITEM("STRING_POWER", GETTEXT("Power"));
	ITEM("STRING_TOTAL_PP", GETTEXT("Total PP"));
	ITEM("STRING_PP_INCOME_IN_THE_PREVIOUS_TURN",
			GETTEXT("PP Income in the Previous Turn"));
	ITEM("STRING_POPULATION", GETTEXT("Population"));
	ITEM("STRING_CONCENTRATION_MODE", GETTEXT("Concentration Mode"));
	ITEM("STRING_COUNCIL", GETTEXT("Council"));
	ITEM("STRING_POSITION__COUNCIL_", GETTEXT("Position (Council)"));
//	ITEM("STRING_EVENT", GETTEXT("Event"));

	ITEM("CHARACTER_NAME", aPlayer->get_nick());

	CRankTable *
		RankTable = PLAYER_TABLE->get_overall_rank_table();
	int
		Rank = RankTable->get_rank_by_id(aPlayer->get_game_id());

	if (Rank >= 1)
	{
		ITEM("RANKING", dec2unit(Rank));
	} else
	{
		ITEM("RANKING", GETTEXT("Not Available"));
	}
	ITEM("POWER", dec2unit(aPlayer->get_power()));
	ITEM("TOTAL_PP", dec2unit(aPlayer->get_production()));
	ITEM("PP_INCOME_IN_THE_PREVIOUS_TURN",
			dec2unit(aPlayer->get_last_turn_production()));
	ITEM("DETAIL_POPULATION", aPlayer->calc_population_with_unit());

	ITEM("DETAIL_CONCENTRATION_MODE", aPlayer->get_mode_name()); 
	ITEM("COUNCIL", aPlayer->get_council()->get_nick());

	CCouncil *
		Council = aPlayer->get_council();
	if (Council->get_speaker_id() == aPlayer->get_game_id())
	{
		ITEM("POSITION__COUNCIL_", GETTEXT("Speaker"));
	} else
	{
		ITEM("POSITION__COUNCIL_", GETTEXT("Member"));
	}

//	ITEM("EVENT", "sdfhhlsdkjfls (in effect)");

	ITEM("STRING_PLANET_STATUS", GETTEXT("Planet Status"));
	ITEM("STRING_RESEARCH_STATUS", GETTEXT("Research Status"));
	ITEM("STRING_FLEET_STATUS", GETTEXT("Fleet Status"));
	ITEM("STRING_COMMANDER_STATUS", GETTEXT("Commander Status"));
	ITEM("STRING_DIPLOMACY_STATUS", GETTEXT("Diplomacy Status"));
	ITEM("STRING_CONTROL_MODEL", GETTEXT("Control Model"));

	ITEM("PLANET_STATUS",
			(char *)format(GETTEXT("Total %1$s planet(s)"),
							dec2unit(aPlayer->get_planet_list()->length())));
	ITEM("RESEARCHED_TECH",
			(char *)format(GETTEXT("Total %1$s researched tech(s)"),
							dec2unit(aPlayer->get_tech_list()->length())));
	int
		TargetTechID = aPlayer->get_target_tech();
	if (TargetTechID == 0)
	{
		ITEM("CURRENTLY_RESEARCHING", GETTEXT("Free Research"));
	}
	else
	{
		CTech *
			TargetTech = (CTech *)TECH_TABLE->get_by_id(TargetTechID);
		CHECK(!TargetTech, GETTEXT("You have an illegal target tech."));

		ITEM("CURRENTLY_RESEARCHING",
				(char *)format(GETTEXT("Currently researching %1$s"),
								TargetTech->get_name()));
	}

	ITEM("TOTAL_FLEET",
			(char *)format(GETTEXT("Total %1$d fleet(s)"),
							aPlayer->get_fleet_list()->length()));
	ITEM("TOTAL_COMMANDER",
			(char *)format(GETTEXT("Total %1$d commander(s)"),
							aPlayer->get_admiral_list()->length() +
							aPlayer->get_admiral_pool()->length()));

	CDiplomaticMessageBox *
		MessageBox = aPlayer->get_diplomatic_message_box();
	ITEM("NEW_MESSAGE",
			(char *)format(
					GETTEXT("%1$s new message(s) arrived."),
					dec2unit(MessageBox->count_unread_message())));

//	system_log("end page handler %s", get_name());

	return output("domestic/whitebook.html");
}
