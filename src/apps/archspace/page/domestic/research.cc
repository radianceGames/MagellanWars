#include <libintl.h>
#include "../../pages.h"
#include "../../archspace.h"
#include "../../game.h"

bool
CPageResearch::handler(CPlayer *aPlayer)
{
//	system_log("start page handler %s", get_name());

	ITEM("STRING_INVESTED_PP_LEFT", GETTEXT("Invested PP Left"));
	ITEM("STRING_MAX_USAGE_PER_TURN", GETTEXT("Maximum PP Usage per Turn"));
	ITEM("STRING_AMOUNT_TO_INVEST", GETTEXT("Amount to Invest"));
	ITEM("STRING_THE_TECH_BEING_RESEARCHED",
			GETTEXT("The Tech Being Researched"));
	ITEM("STRING_RESEARCH_TITLE", GETTEXT("You can research the following tech(s) :"));
	ITEM("STRING_SOCIAL_SCIENCE", GETTEXT("Social Science"));
	ITEM("STRING_INFORMATION_SCIENCE", GETTEXT("Information Science"));
	ITEM("STRING_MATTER_ENERGY_SCIENCE", GETTEXT("Matter-Energy Science"));
	ITEM("STRING_LIFE_SCIENCE", GETTEXT("Life Science"));
	ITEM("STRING_TECH_TREE", GETTEXT("To see the Tech Tree"));
	ITEM("STRING_TECH_ARCHIVED", GETTEXT("To see what you have already achieved"));

	int Invest = aPlayer->get_research_invest();
	ITEM("INVESTED_PP_LEFT", (!Invest) ?
					GETTEXT("None"):dec2unit(Invest));

	int ResearchLab = aPlayer->get_research_lab();
	if (aPlayer->has_ability(ABILITY_EFFICIENT_INVESTMENT)) ResearchLab *= 2;
	ITEM("MAX_USAGE_PER_TURN", (!ResearchLab) ?
					GETTEXT("None"):dec2unit(ResearchLab));

	CString
		TargetResearch;
	CTech
		*Target = NULL;
	if (aPlayer->get_target_tech())
	{
		Target = TECH_TABLE->get_by_id(aPlayer->get_target_tech());
		if (!Target)
		{
			aPlayer->set_target_tech(NONE);
			TargetResearch = GETTEXT("Free Research");
		}
		else
		{
			TargetResearch =
					(char *)format(GETTEXT("Currently researching %1$s"),
									Target->get_name_with_level());
		}
	}
	else
	{
		TargetResearch = GETTEXT("Free Research");
	}
	ITEM("CURRENTLY_RESEARCHING", (char *)TargetResearch);

	if (Target)
	{
		int Progress = (int)((aPlayer->get_research()) /
								aPlayer->get_research_cost(Target))*10;

		CString ProgressBar;
		for(int i=0; i<10; i++)
		{
			ProgressBar.format("<IMG SRC=\"%s/image/as_game/domestic",
								(char *)CGame::mImageServerURL);
			ProgressBar += (i<Progress) ? "/planet_check.gif":"/planet_box.gif";
			ProgressBar += "\" WIDTH=10 HEIGHT=14>";
		}
		ITEM("PROGRESS_BAR", ProgressBar);

		CString Research = dec2unit(aPlayer->get_research());
		CString TotalResearch = dec2unit(aPlayer->get_research_cost(Target));
		ITEM("PROGRESS", format(GETTEXT("( %1$s / %2$s RP)"),
							(char*)Research, (char*)TotalResearch));
	} else {
		ITEM("PROGRESS_BAR", " ");
		//CString Research = dec2unit(aPlayer->get_research());
		ITEM("PROGRESS", format(GETTEXT("No one knows the progress in free research.<BR>You have accumulated %s RP."), dec2unit(aPlayer->get_research())));
	}

	ITEM("SOCIAL_SCIENCE_LIST",
			aPlayer->available_science_list_html(CTech::TYPE_SOCIAL));
	ITEM("INFORMATION_SCIENCE_LIST",
			aPlayer->available_science_list_html(CTech::TYPE_INFORMATION));
	ITEM("MATTER_ENERGY_SCIENCE_LIST",
			aPlayer->available_science_list_html(CTech::TYPE_MATTER_ENERGY));
	ITEM("LIFE_SCIENCE_LIST",
			aPlayer->available_science_list_html(CTech::TYPE_LIFE));
//	system_log("end page handler %s", get_name());

	return output("domestic/research.html");
}
